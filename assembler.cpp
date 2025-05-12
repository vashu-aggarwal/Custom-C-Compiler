#include <iostream>
#include <string>
#include <vector>
#include "parser.h"
#include "assembler.h"

using namespace std;

int find_variable(const vector<string>& variables, const string& name) {
    for (int i = 0; i < variables.size(); ++i) {
        if (variables[i] == name) {
            return i;
        }
    }
    cerr << "Variable not found: " << name << endl;
    exit(1);
}

void term_asm(term_node* term, const vector<string>& variables) {
    switch (term->kind) {
    case TERM_INPUT:
        printf("    read 0, line, LINE_MAX\n");
        printf("    mov rdi, line\n");
        printf("    call strlen\n");
        printf("    mov rdi, line\n");
        printf("    mov rsi, rax\n");
        printf("    call parse_uint\n");
        break;
    case TERM_INT:
        printf("    mov rax, %s\n", term->value.c_str());
        break;
    case TERM_IDENT: {
        int index = find_variable(variables, term->value);
        printf("    mov rax, qword [rbp - %d]\n", index * 8 + 8);
        break;
    }
    }
}

void expr_asm(expr_node* expr, const vector<string>& variables) {
    switch (expr->kind) {
    case EXPR_TERM:
        term_asm(get_if<term_node>(&expr->node_type), variables);
        break;
    case EXPR_PLUS: {
        auto bin = get_if<term_binary_node>(&expr->node_type);
        term_asm(&bin->lhs, variables);
        printf("    mov rdx, rax\n");
        term_asm(&bin->rhs, variables);
        printf("    add rax, rdx\n");
        break;
    }
    }
}

void rel_asm(rel_node* rel, const vector<string>& variables) {
    switch (rel->kind) {
    case REL_LESS_THAN:
        term_asm(&rel->less_than.lhs, variables);
        printf("    mov rdx, rax\n");
        term_asm(&rel->less_than.rhs, variables);
        printf("    cmp rdx, rax\n");
        printf("    setl al\n");
        printf("    and al, 1\n");
        printf("    movzx rax, al\n");
        break;
    }
}

void instr_asm(instr_node* instr, vector<string>& variables, int& if_count) {
    switch (instr->kind) {
    case INSTR_ASSIGN: {
        auto& a = get<assign_node>(instr->node_type);
        expr_asm(&a.expr, variables);
        int index = find_variable(variables, a.ident);
        printf("    mov qword [rbp - %d], rax\n", index * 8 + 8);
        break;
    }
    case INSTR_IF: {
        auto& if_ = get<if_node>(instr->node_type);
        rel_asm(&if_.rel, variables);
        int label = if_count++;
        printf("    test rax, rax\n");
        printf("    jz .endif%d\n", label);
        instr_asm(if_.instr, variables, if_count);
        printf(".endif%d:\n", label);
        break;
    }
    case INSTR_GOTO: {
        auto& g = get<goto_node>(instr->node_type);
        printf("    jmp .%s\n", g.label.c_str());
        break;
    }
    case INSTR_OUTPUT: {
        auto& o = get<output_node>(instr->node_type);
        term_asm(&o.term, variables);
        printf("    mov rdi, 1\n");
        printf("    mov rsi, rax\n");
        printf("    call write_uint\n");
        break;
    }
    case INSTR_LABEL: {
        auto& l = get<label_node>(instr->node_type);
        printf(".%s:\n", l.label.c_str());
        break;
    }
    }
}

void term_declare_variables(term_node* term, const vector<string>& variables) {
    if (term->kind == TERM_IDENT) {
        for (const auto& v : variables) {
            if (term->value == v)
                return;
        }
        cerr << "Identifier not defined: " << term->value << endl;
        exit(1);
    }
}

void expr_declare_variables(expr_node* expr, const vector<string>& variables) {
    switch (expr->kind) {
    case EXPR_TERM:
        term_declare_variables(get_if<term_node>(&expr->node_type), variables);
        break;
    case EXPR_PLUS: {
        auto bin = get_if<term_binary_node>(&expr->node_type);
        term_declare_variables(&bin->lhs, variables);
        term_declare_variables(&bin->rhs, variables);
        break;
    }
    }
}

void rel_declare_variables(rel_node* rel, const vector<string>& variables) {
    term_declare_variables(&rel->less_than.lhs, variables);
    term_declare_variables(&rel->less_than.rhs, variables);
}

void instr_declare_variables(instr_node* instr, vector<string>& variables) {
    switch (instr->kind) {
    case INSTR_ASSIGN: {
        assign_node& a = get<assign_node>(instr->node_type);
        expr_declare_variables(&a.expr, variables);
        for (int i = 0; i < variables.size(); i++) {
            if (variables[i] == a.ident)
                return;
        }
        variables.push_back(a.ident);
        break;
    }
    case INSTR_IF: {
        if_node& i = get<if_node>(instr->node_type);
        rel_declare_variables(&i.rel, variables);
        instr_declare_variables(i.instr, variables);
        break;
    }
    case INSTR_OUTPUT: {
        output_node& o = get<output_node>(instr->node_type);
        term_declare_variables(&o.term, variables);
        break;
    }
    case INSTR_GOTO:
    case INSTR_LABEL:
        break;
    }
}


void program_asm(program_node* program) {
    int if_count = 0;
    vector<string> variables;

    for (int i = 0; i < program->instructions.size(); i++) {
        instr_node* instr = &program->instructions[i];
        instr_declare_variables(instr, variables);
    }

    printf("format ELF64 executable\n");
    printf("LINE_MAX equ 1024\n");
    printf("segment readable executable\n");
    printf("include \"linux.inc\"\n");
    printf("include \"utils.inc\"\n");
    printf("entry _start\n");
    printf("_start:\n");

    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", static_cast<int>(variables.size()) * 8);

    for (int i = 0; i < program->instructions.size(); i++) {
        instr_node* instr = &program->instructions[i];
        instr_asm(instr, variables, if_count);
    }
    printf("    add rsp, %d\n", static_cast<int>(variables.size()) * 8);
    printf("    mov rax, 60\n");
    printf("    xor rdi, rdi\n");
    printf("    syscall\n");
    printf("segment readable writeable\n");
    printf("line rb LINE_MAX\n");
}
