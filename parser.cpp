#include "lexer.h"
#include "parser.h"
using namespace std;

string tokenKindToString(TokenKind kind) {
    switch (kind) {
        case TokenKind::IDENT:      return "Identifier";
        case TokenKind::LABEL:      return "label";
        case TokenKind::INT:        return "int";
        case TokenKind::INPUT:      return "input";
        case TokenKind::OUTPUT:     return "output";
        case TokenKind::GOTO:       return "goto";
        case TokenKind::IF:         return "if";
        case TokenKind::THEN:       return "then";
        case TokenKind::EQUAL:      return "equal";
        case TokenKind::PLUS:       return "plus";
        case TokenKind::LESS_THAN:  return "less than";
        case TokenKind::INVALID:    return "invalid";
        case TokenKind::END:        return "end";
        default:                    return "Unknown";
    }
}

void parser_init(vector<Token> tokens, parser* p) {
    p->tokens = tokens;
    p->index = 0;
}

void parse_term(parser* p, term_node* term) {
    Token token = p->tokens[p->index];
    if (token.kind == TokenKind::INPUT){
        term->kind = TERM_INPUT;
    }
    else if(token.kind == TokenKind::INT){
        term->kind = TERM_INT;
        term->value = token.value;
    }
    else if(token.kind == TokenKind::IDENT){
        term->kind = TERM_IDENT;
        term->value = token.value;
    } 
    else{
        cout<<"Expected int, input or ident but found "<<show_token_kind(token.kind)<<"\n";
        return;
    }

    p->index++;
}

void parse_expr(parser* p, expr_node* expr) {
    Token token;
    term_node lhs, rhs;
    
    parse_term(p, &lhs);
    token = p->tokens[p->index];

    if(token.kind == TokenKind::PLUS){
        p->index++;
        parse_term(p, &rhs);
        expr->kind = EXPR_PLUS;
        expr->node_type = term_binary_node{lhs, rhs};
    } 
    else{
        expr->kind = EXPR_TERM;
        expr->node_type = lhs;
    }
}

void parse_rel(parser* p, rel_node* rel) {
    Token token;
    term_node lhs, rhs;

    parse_term(p, &lhs);
    token = p->tokens[p->index];
    if(token.kind == TokenKind::LESS_THAN){
        p->index++;
        parse_term(p, &rhs);
        rel->kind = REL_LESS_THAN;
        rel->less_than = term_binary_node{lhs, rhs};
    } 
    else{
        cout<<"Expected rel (<) found "<<show_token_kind(token.kind)<<"\n";
        return;
    }
}

void parse_if(parser* p, instr_node* instr) {
    Token token;

    instr->set_kind(INSTR_IF);
    instr->node_type = if_node();
    p->index++;

    auto if_instr = get_if<if_node>(&instr->node_type);
    
    if(!if_instr){
        cerr << "Expected 'if' node, but found different type\n";
        return;
    }

    parse_rel(p, &if_instr->rel);
    token = p->tokens[p->index];
    if(token.kind != TokenKind::THEN){
        cout<<"Expected 'then' but found "<<show_token_kind(token.kind)<<"\n";
        return;
    }
    p->index++;

    if_instr->instr = new instr_node();
    parse_instr(p, if_instr->instr);
}




void parse_assign(parser* p, instr_node* instr) {
    Token token = p->tokens[p->index];

    auto assign_ptr = get_if<assign_node>(&instr->node_type);
    if(assign_ptr)
        assign_ptr->ident = token.value;
    else{
        cerr<<"node_type does not currently hold an assign_node\n";
        return;
    }

    instr->set_kind(INSTR_ASSIGN);
    p->index++;

    if (p->tokens[p->index].kind != TokenKind::EQUAL) {
        cout << "Expected equal but found " << show_token_kind(p->tokens[p->index].kind) << endl;
        return;
    }

    p->index++;
    parse_expr(p, &get<assign_node>(instr->node_type).expr);
}

void parse_goto(parser* p, instr_node* instr) {
    instr->set_kind(INSTR_GOTO);
    instr->node_type = goto_node();
    p->index++;
    
    Token token = p->tokens[p->index];
    
    if(token.kind != TokenKind::LABEL){
        cout<<"Expected label found "<<show_token_kind(token.kind);
    }
    p->index++;

    auto goto_ptr = get_if<goto_node>(&instr->node_type);
    if(goto_ptr){
        goto_ptr->label = token.value;
    } 
    else{
        cerr<<"node_type does not currently hold an goto_node\n";
        return;
    }
}

void parse_output(parser* p, instr_node* instr){
    instr->set_kind(INSTR_OUTPUT);
    instr->node_type = output_node();
    p->index++;

    term_node lhs;
    parse_term(p, &lhs);
    auto output_ptr = get_if<output_node>(&instr->node_type);
    if(output_ptr){
        output_ptr->term = lhs;
    } 
    else{
        cerr << "Error: node_type does not currently hold an output_node\n";
    }
}

void parse_label(parser* p, instr_node* instr){
    instr->set_kind(INSTR_LABEL);
    instr->node_type = label_node();
    Token token = p->tokens[p->index];

    auto label_ptr = get_if<label_node>(&instr->node_type);
    if(label_ptr){
        label_ptr->label = token.value;
    } 
    else{
        cerr << "Error: node_type does not currently hold an label_node\n";
    }  
    p->index++;
}



void parse_instr(parser* p, instr_node* instr){
    Token token = p->tokens[p->index];

    switch (token.kind) {
        case TokenKind::IDENT:
            parse_assign(p, instr);
            break;
        case TokenKind::IF:
            parse_if(p, instr);
            break;
        case TokenKind::GOTO:
            parse_goto(p, instr);
            break;
        case TokenKind::OUTPUT:
            parse_output(p, instr);
            break;
        case TokenKind::LABEL:
            parse_label(p, instr);
            break;
        default:
            cerr << "Unexpected token " << show_token_kind(token.kind);
            break;
    }    
}

void parse_program(struct parser* p, struct program_node* program) {
    struct Token token;
    do{
        struct instr_node instr;
        parse_instr(p, &instr);
        program->instructions.push_back(instr);
        token = p->tokens[p->index];
    }while(token.kind != TokenKind::END);
}

void print_program(struct program_node* program) {
    for (unsigned int i = 0; i < program->instructions.size(); i++) {
        const instr_node& instr = program->instructions[i];
        switch (instr.kind) {
            case INSTR_ASSIGN:{
                auto instr_assign = get_if<assign_node>(&instr.node_type);
                cout<<"<assign> : "<<instr_assign->ident<<"   ";
                switch(instr_assign->expr.kind){
                    case EXPR_TERM:{
                        auto expr_term = get_if<term_node>(&instr_assign->expr.node_type);
                        cout<<expr_term->value<<endl;
                        break;
                    }
                    case EXPR_PLUS:{
                        auto expr_term = get_if<term_binary_node>(&instr_assign->expr.node_type);
                        cout<<expr_term->lhs.value<<" + "<<expr_term->rhs.value<<endl;
                        break;
                    }
                }
                break;
            }
            case INSTR_IF:{
                cout << "<if> \n";
                break;
            }
            case INSTR_GOTO:{
                auto instr_goto = get_if<goto_node>(&instr.node_type);
                cout<<"<goto> : "<<instr_goto->label<<endl;
                break;
            }
            case INSTR_OUTPUT:{
                auto instr_output = get_if<output_node>(&instr.node_type);
                cout<<"<output> : "<<instr_output->term.value<<endl;
                break;
            }
            case INSTR_LABEL:{
                auto instr_label = get_if<label_node>(&instr.node_type);
                cout<<"<label> : "<<instr_label->label<<endl;
                break;
            }
        }
    }
}
