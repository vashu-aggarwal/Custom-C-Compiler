#pragma once
#include <string>
#include <vector>
#include "parser.h"

using namespace std;


void program_asm(program_node* program);


void instr_asm(instr_node* instr, vector<string>& variables, int& if_count);
void expr_asm(expr_node* expr, const vector<string>& variables);
void term_asm(term_node* term, const vector<string>& variables);
void rel_asm(rel_node* rel, const vector<string>& variables);


void instr_declare_variables(instr_node* instr, vector<string>& variables);
void expr_declare_variables(expr_node* expr, const vector<string>& variables);
void term_declare_variables(term_node* term, const vector<string>& variables);
void rel_declare_variables(rel_node* rel, const vector<string>& variables);
