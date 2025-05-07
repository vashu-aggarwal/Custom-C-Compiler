#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <variant>
#include <memory>

#include "lexer.h"

using namespace std;

enum term_kind { 
    TERM_INPUT, 
    TERM_INT, 
    TERM_IDENT 
};

enum instr_kind { 
    INSTR_ASSIGN, 
    INSTR_IF, 
    INSTR_GOTO, 
    INSTR_OUTPUT, 
    INSTR_LABEL 
};

enum expr_kind { 
    EXPR_TERM, 
    EXPR_PLUS 
};

enum rel_kind {
    REL_LESS_THAN 
};


struct instr_node; // forward declaration of instr_node

struct term_node {
    term_kind kind;
    string value;
};

struct term_binary_node {
    term_node lhs;
    term_node rhs;
};

struct expr_node {
    expr_kind kind;
    variant<term_node, 
            term_binary_node> node_type;

    expr_node() : // constructor needed, since variant node_type always requires to hold a valid value (here bydefault EXPR_TERM)
        kind(EXPR_TERM), 
        node_type(term_node()) {}
};

struct rel_node {
    rel_kind kind;
    term_binary_node less_than;
};

struct assign_node {
    string ident;
    expr_node expr;
};

struct if_node {
    rel_node rel;
    instr_node* instr; // needed to point to the goto instruction present in the if instruction
    // if_node.instr will point to a dynamically parsed goto instruction
};

struct output_node {
    term_node term;
};

struct goto_node {
    string label;
};

struct label_node {
    string label;
};

struct instr_node {
    instr_kind kind;
    variant<assign_node,
        if_node,
        goto_node,
        output_node,
        label_node> node_type;
    // instead of union, variant(type-safe union) is used, which automatically handles construction, destruction and type checks unline unions or struct

    instr_node() : // default constructor
        kind(INSTR_ASSIGN), 
        node_type(assign_node()) {}
    
    instr_node(instr_kind k) : // parameterized constructor
        kind(k) {
        switch (k) {
            case INSTR_ASSIGN: new(&node_type) assign_node(); break;
            case INSTR_IF: new(&node_type) if_node(); break;
            case INSTR_GOTO: new(&node_type) goto_node(); break;
            case INSTR_OUTPUT: new(&node_type) output_node(); break;
            case INSTR_LABEL: new(&node_type) label_node(); break;
        }
    }

    void set_kind(instr_kind k) { // when a new instruction is being parsed
        if (kind == k) return;
        kind = k;
    }
};

struct program_node {
    vector<instr_node> instructions;
};

struct parser {
    vector<Token> tokens;
    unsigned int index;
};

void parser_init(vector<Token> tokens, parser* p);
void parse_term(parser* p, term_node* term);
void parse_expr(parser* p, expr_node* expr);
void parse_rel(parser* p, rel_node* rel);
void parse_if(parser* p, instr_node* instr);
void parse_assign(parser* p, instr_node* instr);
void parse_goto(parser* p, instr_node* instr);
void parse_output(parser* p, instr_node* instr);
void parse_label(parser* p, instr_node* instr);
void parse_instr(parser* p, instr_node* instr);
void parse_program(parser* p, program_node* program);
void print_program(struct program_node* program);
