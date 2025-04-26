#pragma once

#include <string>
#include <vector>

// Token kinds
enum class TokenKind {
    IDENT,
    LABEL,
    INT,
    INPUT,
    OUTPUT,
    GOTO,
    IF,
    THEN,
    EQUAL,
    PLUS,
    LESS_THAN,
    INVALID,
    END
};

struct Token {
    TokenKind kind;
    std::string value;
};

std::string show_token_kind(TokenKind kind);
void print_token(const Token& token);

// Lexer class
class Lexer {
public:
    explicit Lexer(const std::string& input);
    Token next_token();
private:
    std::string buffer;
    size_t pos;
    size_t read_pos;
    char ch;

    void read_char();
    void skip_whitespace();
    std::string read_identifier();
    std::string read_number();
};  