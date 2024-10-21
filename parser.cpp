#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>  // For file handling
#include <sstream>  // For reading file content

using namespace std;

enum TokenType {
    T_INT, T_FLOAT, T_DOUBLE, T_CHAR, T_BOOL, T_STRING, T_VOID, T_ID, T_NUM, T_IF, T_ELSE, T_WHILE,
    T_FOR, T_BREAK, T_CONTINUE, T_RETURN, T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,  
    T_SEMICOLON, T_GT, T_EOF, T_LG,
};

struct Token {
    TokenType type;
    string value;
    int line; // Add line number information
};

class Lexer {
private:
    string src;
    size_t pos;
    int line;  // Line number tracking

public:
    Lexer(const string &src) {
        this->src = src;
        this->pos = 0;
        this->line = 1;  // Start on line 1
    }

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (pos < src.size()) {
            char current = src[pos];
            
            if (isspace(current)) {
                if (current == '\n') line++;  // Increment line on newline
                pos++;
                continue;
            }
            if (isdigit(current)) {
                tokens.push_back(Token{T_NUM, consumeNumber(), line});
                continue;
            }
            if (isalpha(current)) {
                string word = consumeWord();
                if (word == "int") tokens.push_back(Token{T_INT, word, line});
                else if (word == "float") tokens.push_back(Token{T_FLOAT, word, line});
                else if (word == "double") tokens.push_back(Token{T_DOUBLE, word, line});
                else if (word == "char") tokens.push_back(Token{T_CHAR, word, line});
                else if (word == "bool") tokens.push_back(Token{T_BOOL, word, line});
                else if (word == "string") tokens.push_back(Token{T_STRING, word, line});
                else if (word == "void") tokens.push_back(Token{T_VOID, word, line});
                else if (word == "if") tokens.push_back(Token{T_IF, word, line});
                else if (word == "else") tokens.push_back(Token{T_ELSE, word, line});
                else if (word == "while") tokens.push_back(Token{T_WHILE, word, line});
                else if (word == "for") tokens.push_back(Token{T_FOR, word, line});
                else if (word == "break") tokens.push_back(Token{T_BREAK, word, line});
                else if (word == "continue") tokens.push_back(Token{T_CONTINUE, word, line});
                else if (word == "return") tokens.push_back(Token{T_RETURN, word, line});
                else tokens.push_back(Token{T_ID, word, line});
                continue;
            }
            
            switch (current) {
                case '=': tokens.push_back(Token{T_ASSIGN, "=", line}); break;
                case '+': tokens.push_back(Token{T_PLUS, "+", line}); break;
                case '-': tokens.push_back(Token{T_MINUS, "-", line}); break;
                case '*': tokens.push_back(Token{T_MUL, "*", line}); break;
                case '/': tokens.push_back(Token{T_DIV, "/", line}); break;
                case '(': tokens.push_back(Token{T_LPAREN, "(", line}); break;
                case ')': tokens.push_back(Token{T_RPAREN, ")", line}); break;
                case '{': tokens.push_back(Token{T_LBRACE, "{", line}); break;  
                case '}': tokens.push_back(Token{T_RBRACE, "}", line}); break;  
                case ';': tokens.push_back(Token{T_SEMICOLON, ";", line}); break;
                case '>': tokens.push_back(Token{T_GT, ">", line}); break;
                case '<': tokens.push_back(Token{T_LG, "<",line});break;
                default: 
                    cout << "Unexpected character: " << current << " at line " << line << endl; 
                    exit(1);
            }
            pos++;
        }
        tokens.push_back(Token{T_EOF, "", line});
        return tokens;
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
};

class Parser {
public:
    Parser(const vector<Token> &tokens) {
        this->tokens = tokens;
        this->pos = 0;
    }

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;

    void parseStatement() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || 
            tokens[pos].type == T_DOUBLE || tokens[pos].type == T_CHAR || 
            tokens[pos].type == T_BOOL || tokens[pos].type == T_STRING || 
            tokens[pos].type == T_VOID) {
            parseDeclaration();
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_IF) {
            parseIfStatement();
        } else if (tokens[pos].type == T_WHILE) {
            parseWhileStatement();
        } else if (tokens[pos].type == T_FOR) {
            parseForStatement();
        } else if (tokens[pos].type == T_BREAK) {
            parseBreakStatement();
        } else if (tokens[pos].type == T_CONTINUE) {
            parseContinueStatement();
        } else if (tokens[pos].type == T_RETURN) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_LBRACE) {
            parseBlock();
        } else {
            cout << "Syntax error: unexpected token '" << tokens[pos].value
                 << "' at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void parseBlock() {
        expect(T_LBRACE, "{");
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE, "}");
    }

    void parseDeclaration() {
        if (tokens[pos].type == T_INT) {
            expect(T_INT, "int");
        } else if (tokens[pos].type == T_FLOAT) {
            expect(T_FLOAT, "float");
        } else if (tokens[pos].type == T_DOUBLE) {
            expect(T_DOUBLE, "double");
        } else if (tokens[pos].type == T_CHAR) {
            expect(T_CHAR, "char");
        } else if (tokens[pos].type == T_BOOL) {
            expect(T_BOOL, "bool");
        } else if (tokens[pos].type == T_STRING) {
            expect(T_STRING, "string");
        } else if (tokens[pos].type == T_VOID) {
            expect(T_VOID, "void");
        }

        expect(T_ID, "identifier");
        expect(T_SEMICOLON, ";");
    }

    void parseAssignment() {
        expect(T_ID, "identifier");
        expect(T_ASSIGN, "=");
        parseExpression();
        expect(T_SEMICOLON, ";");
    }

    void parseIfStatement() {
        expect(T_IF, "if");
        expect(T_LPAREN, "(");
        parseExpression();
        expect(T_RPAREN, ")");
        parseStatement();
        if (tokens[pos].type == T_ELSE) {
            expect(T_ELSE, "else");
            parseStatement();
        }
    }

    void parseWhileStatement() {
        expect(T_WHILE, "while");
        expect(T_LPAREN, "(");
        parseExpression();
        expect(T_RPAREN, ")");
        parseStatement();
    }

    void parseForStatement() {
        expect(T_FOR, "for");
        expect(T_LPAREN, "(");
        parseExpression();
        expect(T_SEMICOLON, ";");
        parseExpression();
        expect(T_SEMICOLON, ";");
        parseExpression();
        expect(T_RPAREN, ")");
        parseStatement();
    }

    void parseBreakStatement() {
        expect(T_BREAK, "break");
        expect(T_SEMICOLON, ";");
    }

    void parseContinueStatement() {
        expect(T_CONTINUE, "continue");
        expect(T_SEMICOLON, ";");
    }

    void parseReturnStatement() {
        expect(T_RETURN, "return");
        parseExpression();
        expect(T_SEMICOLON, ";");
    }

    void parseExpression() {
        parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            pos++;
            parseTerm();
        }
        if (tokens[pos].type == T_GT) {
            pos++;
            parseTerm();
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
        if (tokens[pos].type == T_NUM) {
            pos++;
        } else if (tokens[pos].type == T_LPAREN) {
            expect(T_LPAREN, "(");
            parseExpression();
            expect(T_RPAREN, ")");
        } else if (tokens[pos].type == T_ID) {
            pos++;
        } else {
            cout << "Syntax error at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void expect(TokenType expectedType, const string &tokenName) {
        if (tokens[pos].type != expectedType) {
            cout << "Expected " << tokenName << " but found '"
                 << tokens[pos].value << "' at line " << tokens[pos].line << endl;
            exit(1);
        }
        pos++;
    }
};

int main() {
    string filePath = "Parser.txt";
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "Failed to open file: " << filePath << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string src = buffer.str();

    Lexer lexer(src);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}