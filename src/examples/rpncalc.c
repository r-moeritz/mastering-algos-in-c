#include "../../include/stack.h"
#include "../../include/queue.h"
#include <stdio.h>
#include <string.h>

typedef enum TokenKind_ {
    OPERATOR,
    OPERAND
} TokenKind;

typedef struct Token_ {
    TokenKind kind;
    int value;
} Token;

typedef enum Operator_ {
    ADDITION = 1,
    SUBTRACTION = 2,
    MULTIPLICATION = 3,
    DIVISION = 4
} Operator;

int read_line(FILE* stream, char* buffer, int size);

int tokenize(char* str, Queue* tokens);

Token* parse(const char* str);

void eval(const Token* token, Stack* s);

int apply(Operator operator, int operand1, int operand2);

Token* new_token(TokenKind kind, int value);

int main(int argc, char* argv[]) {
    Stack s;
    Queue tokens;
    char buffer[128];
    char* str;
    Token* tok;
    int* res;

    queue_init(&tokens, free);
    stack_init(&s, free);

    for (;;) {
        if (read_line(stdin, buffer, sizeof(buffer))) {
            fputs("Error reading input!\n", stderr);
            break;
        }

        if (tokenize(buffer, &tokens)) {
            fputs("Error tokenizing input!\n", stderr);
            break;
        }

        while (!queue_dequeue(&tokens, (void**) &str)) {
            tok = parse(str);
            if (!tok) {
                fputs("Error parsing token!\n", stderr);
                break;
            }

            eval(tok, &s);
        }

        if (!stack_peek(&s)) {
            break;
        }

        stack_pop(&s, (void** ) &res);
        printf("%d\n", *res);
        free(res);
    }

    return 0;
}

int read_line(FILE* stream, char* buffer, int size) {
    fgets(buffer, size - 1, stream);
    buffer[strlen(buffer) - 1] = '\0'; // remove newline
    return 0;
}

int tokenize(char* str, Queue* tokens) {
    char* ws = " ";
    char* s = strtok(str, ws);
    while (s) {
        queue_enqueue(tokens, s);
        s = strtok(NULL, ws);
    }
    return 0;
}

Token* parse(const char* str) {
    TokenKind kind;
    int value;

    if (!strcmp("+", str)) {
        kind = OPERATOR;
        value = ADDITION;
    }
    else if (!strcmp("-", str)) {
        kind = OPERATOR;
        value = SUBTRACTION;
    }
    else if (!strcmp("/", str)) {
        kind = OPERATOR;
        value = DIVISION;
    }
    else if (!strcmp("*", str)) {
        kind = OPERATOR;
        value = MULTIPLICATION;
    }
    else {
        kind = OPERAND;
        value = atoi(str);
    }

    return new_token(kind, value);
}

void eval(const Token* tok, Stack* s) {
    int* mem = NULL;
    int* n;

    if (tok->kind == OPERAND) {
        n = malloc(sizeof(int));
        if (!n) {
            return;
        }
        *n = tok->value;
        stack_push(s, n);
    }
    else {
        while (stack_peek(s)) {
            stack_pop(s, (void**) &n);

            if (mem) {
                *mem = apply(tok->value, *n, *mem);
                free(n);
            }
            else {
                mem = n;
            }
        }

        if (mem) {
            stack_push(s, mem);
        }
    }
}

int apply(Operator operator, int operand1, int operand2) {
    switch (operator) {
        case ADDITION:
            return operand1 + operand2;

        case SUBTRACTION:
            return operand1 - operand2;

        case MULTIPLICATION:
            return operand1 * operand2;

        case DIVISION:
            return operand1 / operand2;
    }

    return 0;
}

Token* new_token(TokenKind kind, int value) {
    Token* tok = malloc(sizeof(Token));
    if (!tok) {
        return NULL;
    }

    tok->kind = kind;
    tok->value = value;

    return tok;
}
