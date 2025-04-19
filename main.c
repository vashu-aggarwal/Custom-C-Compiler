// Command to run this code
// gcc main.c -o main
// type .\example.txt | .\main

#include <ctype.h>
#include <stdio.h>
#define DS_IMPLEMENTATION
#include "ds.h"
#include <string.h>

// possible tokens we can have in the language
enum token_kind
{
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
// this is going to help us read the next token
struct token
{
    enum token_kind kind;
    char *value;
};

const char *show_token_kind(enum token_kind kind)
{
    switch (kind)
    {
    case IDENT:
        return "ident";
    case LABEL:
        return "label";
    case INT:
        return "int";
    case INPUT:
        return "input";
    case OUTPUT:
        return "output";
    case GOTO:
        return "goto";
    case IF:
        return "if";
    case THEN:
        return "then";
    case EQUAL:
        return "equal";
    case PLUS:
        return "plus";
    case LESS_THAN:
        return "less_than";
    case INVALID:
        return "invalid";
    case END:
        return "end";
    }
}

// printing in the readable formate
void print_token(struct token tok)
{
    const char *kind = show_token_kind(tok.kind);
    printf("%s", kind);
    if (tok.value != NULL)
    {
        printf("(%s)", tok.value);
    }
    printf("\n");
}

// lexer is created
struct lexer
{
    char *buffer;
    unsigned int buffer_len;
    unsigned int pos;
    unsigned int read_pos;
    char ch;
};
// Read the next character and tells you what character is there
static char lexer_peek_char(struct lexer *l)
{
    if (l->read_pos >= l->buffer_len)
    {
        return EOF;
    }

    return l->buffer[l->read_pos];
}
// Each time character got read pointer advances its a kind of an iterator which is taking us to a next character position
static char lexer_read_char(struct lexer *l)
{
    l->ch = lexer_peek_char(l);

    l->pos = l->read_pos;
    l->read_pos += 1;

    return l->ch;
}

static void skip_whitespaces(struct lexer *l)
{
    while (isspace(l->ch))
    {
        lexer_read_char(l);
    }
}

// intializing the buffer
static void lexer_init(struct lexer *l, char *buffer, unsigned int buffer_len)
{
    l->buffer = buffer;
    l->buffer_len = buffer_len;
    l->pos = 0;
    l->read_pos = 0;
    l->ch = 0;

    lexer_read_char(l);
}

// being able to read the next token
static struct token lexer_next_token(struct lexer *l)
{
    skip_whitespaces(l);
    if (l->ch == EOF)
    {
        lexer_read_char(l);
        return (struct token){.kind = END, .value = NULL};
    }
    else if (l->ch == '=')
    {
        lexer_read_char(l);
        return (struct token){.kind = EQUAL, .value = NULL};
    }
    else if (l->ch == '+')
    {
        lexer_read_char(l);
        return (struct token){.kind = PLUS, .value = NULL};
    }
    else if (l->ch == '<')
    {
        lexer_read_char(l);
        return (struct token){.kind = LESS_THAN, .value = NULL};
    }
    else if (l->ch == ':')
    {
        // read  till the end when : comes
        lexer_read_char(l);
        ds_string_slice slice = {.str = l->buffer + l->pos, .len = 0};
        while (isalnum(l->ch) || l->ch == '_')
        {
            slice.len += 1;
            lexer_read_char(l);
        }
        char *value = NULL;
        ds_string_slice_to_owned(&slice, &value);
        return (struct token){.kind = LABEL, .value = value};
    }
    else if (isdigit(l->ch))
    {
        // read till the digit end
        ds_string_slice slice = {.str = l->buffer + l->pos, .len = 0};
        while (isdigit(l->ch))
        {
            slice.len += 1;
            lexer_read_char(l);
        }
        char *value = NULL;
        ds_string_slice_to_owned(&slice, &value);
        return (struct token){.kind = INT, .value = value};
    }
    else if (isalnum(l->ch) || l->ch == '_')
    {
        // starts with letter or underscore
        ds_string_slice slice = {.str = l->buffer + l->pos, .len = 0};
        while (isalnum(l->ch) || l->ch == '_')
        {
            slice.len += 1;
            lexer_read_char(l);
        }
        char *value = NULL;
        ds_string_slice_to_owned(&slice, &value);
        if (strcmp(value, "input") == 0)
        {
            return (struct token){.kind = INPUT, .value = NULL};
        }
        else if (strcmp(value, "output") == 0)
        {
            return (struct token){.kind = OUTPUT, .value = NULL};
        }
        else if (strcmp(value, "goto") == 0)
        {
            return (struct token){.kind = GOTO, .value = NULL};
        }
        else if (strcmp(value, "if") == 0)
        {
            return (struct token){.kind = IF, .value = NULL};
        }
        else if (strcmp(value, "then") == 0)
        {
            return (struct token){.kind = THEN, .value = NULL};
        }
        else
        {
            return (struct token){.kind = IDENT, .value = value};
        }
    }
    else
    {
        ds_string_slice slice = {.str = l->buffer + l->pos, .len = 1};
        char *value = NULL;
        ds_string_slice_to_owned(&slice, &value);
        lexer_read_char(l);
        return (struct token){.kind = INVALID, .value = value};
    }
}

int lexer_tokenize(char *buffer, unsigned int length, ds_dynamic_array *tokens)
{
    struct lexer lexer;
    // initializing a lexer
    lexer_init(&lexer, (char *)buffer, length);

    struct token tok;
    do
    {
        tok = lexer_next_token(&lexer);
        if (ds_dynamic_array_append(tokens, &tok) != 0)
        {
            DS_PANIC("Failed to append token to array");
        }
    } while (tok.kind != END);

    return 0;
}

int main()
{
    char *buffer = NULL;
    int length = ds_io_read_file(NULL, &buffer);
    printf("%d\n", length);
    ds_dynamic_array tokens;
    // store tokens inside this
    ds_dynamic_array_init(&tokens, sizeof(struct token));
    lexer_tokenize(buffer, length, &tokens);
    printf("%d\n", tokens.count);
    for (unsigned int i = 0; i < tokens.count; i++)
    {
        struct token tok;
        ds_dynamic_array_get(&tokens, i, &tok);
        print_token(tok);
    }
    // printf("%s\n", buffer);
}