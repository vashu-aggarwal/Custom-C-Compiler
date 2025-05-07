#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

string read_input() {
    ostringstream ss;
    ss << cin.rdbuf();
    return ss.str();
}

int main() {
    string input = read_input();
    Lexer lexer(input);
    vector<Token> tokens;

    Token tok;
    do{
        tok = lexer.next_token();
        tokens.push_back(tok);
    }while (tok.kind != TokenKind::END);

    cout << tokens.size() << "\n";
    for (const auto& t : tokens) {
        print_token(t);
    }
    cout<<"\n\n --------------PARSER-----------\n";
    struct parser p;
    struct program_node program;
    parser_init(tokens,&p);

    parse_program(&p, &program);

    print_program(&program);
    return 0;
}
