#include "lexer.h"
#include <cctype>
#include <iostream>
#include <sstream>

Lexer::Lexer(const std::string& input)
    : buffer(input), pos(0), read_pos(0), ch(0) {
    read_char();
}

Token Lexer::next_token() {
    skip_whitespace();

    if (ch == '\0') return {TokenKind::END, ""};

    if (ch == '=') { read_char(); return {TokenKind::EQUAL, ""}; }
    if (ch == '+') { read_char(); return {TokenKind::PLUS, ""}; }
    if (ch == '<') { read_char(); return {TokenKind::LESS_THAN, ""}; }

    if (ch == ':') {
        read_char();
        std::string label = read_identifier();
        return {TokenKind::LABEL, label};
    }

    if (isdigit(ch)) {
        return {TokenKind::INT, read_number()};
    }

    if (isalnum(ch) || ch == '_') {
        std::string ident = read_identifier();
        if (ident == "input") return {TokenKind::INPUT, ""};
        if (ident == "output") return {TokenKind::OUTPUT, ""};
        if (ident == "goto") return {TokenKind::GOTO, ""};
        if (ident == "if") return {TokenKind::IF, ""};
        if (ident == "then") return {TokenKind::THEN, ""};
        return {TokenKind::IDENT, ident};
    }

    std::string invalid(1, ch);
    read_char();
    return {TokenKind::INVALID, invalid};
}

void Lexer::read_char() {
    if (read_pos >= buffer.size()) {
        ch = '\0';
    } else {
        ch = buffer[read_pos];
    }
    pos = read_pos++;
}

void Lexer::skip_whitespace() {
    while (isspace(ch)) {
        read_char();
    }
}

std::string Lexer::read_identifier() {
    size_t start = pos;
    while (isalnum(ch) || ch == '_') {
        read_char();
    }
    return buffer.substr(start, pos - start);
}

std::string Lexer::read_number() {
    size_t start = pos;
    while (isdigit(ch)) {
        read_char();
    }
    return buffer.substr(start, pos - start);
}

std::string show_token_kind(TokenKind kind) {
    switch (kind) {
        case TokenKind::IDENT: return "ident";
        case TokenKind::LABEL: return "label";
        case TokenKind::INT: return "int";
        case TokenKind::INPUT: return "input";
        case TokenKind::OUTPUT: return "output";
        case TokenKind::GOTO: return "goto";
        case TokenKind::IF: return "if";
        case TokenKind::THEN: return "then";
        case TokenKind::EQUAL: return "equal";
        case TokenKind::PLUS: return "plus";
        case TokenKind::LESS_THAN: return "less_than";
        case TokenKind::INVALID: return "invalid";
        case TokenKind::END: return "end";
        default: return "unknown";
    }
}

void print_token(const Token& token) {
    std::cout << show_token_kind(token.kind);
    if (!token.value.empty()) {
        std::cout << "(" << token.value << ")";
    }
    std::cout << "\n";
}
