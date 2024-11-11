// Add loop feature into your language (while or for)
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <unordered_map>

using namespace std;

enum TokenType {
    T_INT, T_FLOAT, T_CHAR, T_ID, T_NUM, 
    T_AGAR, T_ELSE, T_WHILE, T_FOR, T_VOID, T_RETURN, 
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,  
    T_SEMICOLON, T_GT, T_LT, T_EOF, T_COMMA, T_PERIOD, T_PRINT, 
    T_IF,
};

struct Token {
    TokenType type;
    string value;
    int line;  // Line number for the token
};

struct Symbol {
    string type;  // Type of the identifier (e.g., "int", "float")
    string name;  // Name of the identifier
    int scopeLevel;  // Scope level, if needed (for nested scopes)
    Symbol() : type(""), name(""), scopeLevel(0) {}
    Symbol(string t, string n, int s) : type(t), name(n), scopeLevel(s) {}
};

class SymbolTable {
private:
    unordered_map<string, Symbol> table;

public:
    void addSymbol(const string& name, const string& type, int scopeLevel) {
        if (table.find(name) != table.end()) {
            cerr << "Error: Variable '" << name << "' is already defined." << endl;
            exit(1);
        }
        table[name] = Symbol(type, name, scopeLevel);
    }

    Symbol* getSymbol(const string& name) {
        if (table.find(name) != table.end()) {
            return &table[name];
        }
        return nullptr;  // Return nullptr if the symbol is not found
    }

    bool symbolExists(const string& name) {
        return table.find(name) != table.end();
    }
};

class Lexer {
private:
    string src;
    size_t pos;
    int lineNumber;  // Line number tracker

public:
    Lexer(const string &src) : src(src), pos(0), lineNumber(1) {}

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (pos < src.size()) {
            char current = src[pos];
            
            if (isspace(current)) {
                if (current == '\n') lineNumber++;
                pos++;
                continue;
            }
             // Skip single-line comments
            if (current == '/' && src[pos+1] == '/') {
                skipSingleLineComment();
                continue;
            }
            // Skip multi-line comments
            if (current == '/' && src[pos+1] == '*') {
                skipMultiLineComment();
                continue;
            }
            if (isdigit(current)) {
                tokens.push_back(Token{T_NUM, consumeNumber(), lineNumber});
                continue;
            }
            if (isalpha(current)) {
                string word = consumeWord();
                TokenType tokenType = identifyKeyword(word);
                tokens.push_back(Token{tokenType, word, lineNumber});
                continue;
            }
            
            switch (current) {
                case '=': tokens.push_back(Token{T_ASSIGN, "=", lineNumber}); break;
                case '+': tokens.push_back(Token{T_PLUS, "+", lineNumber}); break;
                case '-': tokens.push_back(Token{T_MINUS, "-", lineNumber}); break;
                case ' ': tokens.push_back(Token{T_MUL, " ", lineNumber}); break;
                case '/': tokens.push_back(Token{T_DIV, "/", lineNumber}); break;
                case '(': tokens.push_back(Token{T_LPAREN, "(", lineNumber}); break;
                case ')': tokens.push_back(Token{T_RPAREN, ")", lineNumber}); break;
                case '{': tokens.push_back(Token{T_LBRACE, "{", lineNumber}); break;  
                case '}': tokens.push_back(Token{T_RBRACE, "}", lineNumber}); break;  
                case ';': tokens.push_back(Token{T_SEMICOLON, ";", lineNumber}); break;
                case '>': tokens.push_back(Token{T_GT, ">", lineNumber}); break;
                case '<': tokens.push_back(Token{T_LT, "<", lineNumber}); break;
                case ',': tokens.push_back(Token{T_COMMA, ",", lineNumber}); break;
                case '.': tokens.push_back(Token{T_PERIOD, ".", lineNumber}); break;
                default: {
                    cout << "Error: Unexpected character '" << current << "' at line " << lineNumber << endl;
                    exit(1);
                }
            }
            pos++;
        }
        tokens.push_back(Token{T_EOF, "", lineNumber}); // EOF token
        return tokens;
    }
    void skipSingleLineComment()
    {
        while (pos<src.size()&& src[pos]!='\n')
        {
            pos++;
        }
    }
    
    void skipMultiLineComment() {
    pos += 2;  // Skip the opening '/*'
    while (pos < src.size()) {
        if (src[pos] == '*' && src[pos + 1] == '/') {
            pos += 2;  // Skip the closing '*/'
            return;
        }
        if (src[pos] == '\n') {
            lineNumber++;  // Track newlines to maintain the line count
        }
        pos++;
    }
    // If the loop ends without finding '*/', it's an unterminated comment
    cerr << "Error: Unterminated multi-line comment starting at line " << lineNumber << endl;
    exit(1);
    }

    string consumeNumber() {
        size_t start = pos;
        while (pos < src.size() && isdigit(src[pos])) pos++;
        return src.substr(start, pos - start);
    }

    string consumeWord() {
        size_t start = pos;
        while (pos < src.size() && isalnum(src[pos])) pos++;
        return src.substr(start, pos - start);
    }

    TokenType identifyKeyword(const string& word) {
        if (word == "int") return T_INT;
        if (word == "float") return T_FLOAT;
        if (word == "char") return T_CHAR;
        if (word == "agar") return T_AGAR;
        if (word == "else") return T_ELSE;
        if (word == "while") return T_WHILE;
        if (word == "for") return T_FOR;
        if (word == "void") return T_VOID;
        if (word == "return") return T_RETURN;
        if (word == "print") return T_PRINT;
        if (word == "if") return T_IF;
        return T_ID;
    }
};

class Parser {
public:
    Parser(const vector<Token>& tokens, SymbolTable& symbolTable) : tokens(tokens), SymbolTable(symbolTable), pos(0) {}
    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;
    SymbolTable& SymbolTable;

    void parseStatement() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_CHAR) {
            parseDeclaration();
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_AGAR) {
            parseIfStatement();
        } else if (tokens[pos].type == T_RETURN) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_WHILE) {
            parseWhileStatement();
        } else if (tokens[pos].type == T_FOR) {
            parseForStatement();
        } else if (tokens[pos].type == T_PRINT) {
            parsePrintStatement();
        } else if (tokens[pos].type == T_LBRACE) {
            parseBlock();
        } else {
            cout<<"unexpected token" <<tokens[pos].value <<"on line number" <<tokens[pos].line ;
            exit(1);
        }
    }

    void parseBlock() {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseDeclaration() {
        expect(tokens[pos].type); // Expect type (int, float, char)
        expect(T_ID);
        expect(T_SEMICOLON);
    }
     void parseinitialize() {
        expect(tokens[pos].type); // Expect type (int, float, char)
        parseAssignment();
        parseExpression();
        expect(T_SEMICOLON);
    }
    void parsePrintStatement() {
        expect(T_PRINT);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        expect(T_SEMICOLON);
    }

    void parseAssignment() {
        expect(T_ID);
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseIfStatement() {
        expect(T_AGAR);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
        if (tokens[pos].type == T_ELSE) {
            expect(T_ELSE);
            parseStatement();
        }
    }

    void parseReturnStatement() {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseWhileStatement() {
        expect(T_WHILE);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
    }

    // void parseForStatement() {
    //     expect(T_FOR);
    //     expect(T_LPAREN);
    //     // parseDeclaration();
    //     // parseAssignment(); // For simplicity, we expect an assignment for the initialization
    //     // Add more parsing for condition and increment if needed
    //     parseinitialize();
    //     parseExpression();
    //     expect(T_SEMICOLON);
    //     parseAssignment();
    //     expect(T_RPAREN);
    //     parseStatement();
    // }
    void parseForStatement() {
        expect(T_FOR);
        expect(T_LPAREN);

        // Parse initialization (optional)
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_CHAR) {
            parseDeclaration();  // Parse declaration, like int i ;
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();  // Parse assignment, like i = 0;
        } else {
            expect(T_SEMICOLON);  // If no declaration or assignment, expect a semicolon
        }

        // Parse condition (mandatory)
        parseExpression();  // Parse condition expression
        expect(T_SEMICOLON);  // Expect semicolon after condition

        // Parse increment (optional)
        if (tokens[pos].type == T_ID) {
            parseAssignment();  // Parse increment, like i = i + 1;
        }

        expect(T_RPAREN);
        parseStatement();  // Parse the body of the for loop
    }


    void parseExpression() {
        parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            pos++;  // Consume the operator
            parseTerm();  // Parse the next term
        }
        if (tokens[pos].type == T_GT || tokens[pos].type == T_LT) {
            pos++;
            parseExpression();
        }
    }

    void parseTerm() {
        parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            pos++;
            parseFactor();
        }
    }

    void parseFactor() {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_ID) {
            pos++;
        } else if (tokens[pos].type == T_LPAREN) {
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        } else {
            cout<<"unexpected token" <<tokens[pos].value <<"on line number" <<tokens[pos].line ;
            exit(1);
        }
    }

    void expect(TokenType type) {
        if (tokens[pos].type == type) {
            pos++;
        } else {
            cout<<"expected " << tokenTypeToString(type) << " but found " << tokens[pos].value<<"on line number" <<tokens[pos].line;
        }
    }


    string tokenTypeToString(TokenType type) {
        switch (type) {
            case T_INT: return "int";
            case T_FLOAT: return "float";
            case T_ID: return "identifier";
            case T_NUM: return "number";
            case T_AGAR: return "if";
            case T_ELSE: return "else";
            case T_RETURN: return "return";
            case T_ASSIGN: return "=";
            case T_PLUS: return "+";
            case T_MINUS: return "-";
            case T_MUL: return "*";
            case T_DIV: return "/";
            case T_LPAREN: return "(";
            case T_RPAREN: return ")";
            case T_LBRACE: return "{";
            case T_RBRACE: return "}";
            case T_SEMICOLON: return ";";
            case T_GT: return ">";
            case T_WHILE: return "while";  // Added while keyword
            case T_FOR: return "for";  
            case T_EOF: return "EOF";
            default: return "unknown";
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file) {
        cout << "Error: Cannot open file " << argv[1] << endl;
        return 1;
    }

    string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    
    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();
    
    SymbolTable symbolTable;  // Declare a SymbolTable object
    Parser parser(tokens, symbolTable);  // Pass symbolTable to the Parser constructor
    parser.parseProgram();

    return 0;
}