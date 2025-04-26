#include "lexer.h"
#include <iostream>
#include <sstream>
#include <vector>

std::string read_input() {
    std::ostringstream ss;
    ss << std::cin.rdbuf();
    return ss.str();
}

int main() {
    std::string input = read_input();
    Lexer lexer(input);
    std::vector<Token> tokens;

    Token tok;
    do{
        tok = lexer.next_token();
        tokens.push_back(tok);
    }while (tok.kind != TokenKind::END);

    std::cout << tokens.size() << "\n";
    for (const auto& t : tokens) {
        print_token(t);
    }
    return 0;
}
