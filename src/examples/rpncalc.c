#include "../../include/stack.h"
#include "../../include/queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief The kind of token
 */
typedef enum TokenKind_ {
    NULLARY_OPERATOR,
    UNARY_OPERATOR,
    BINARY_OPERATOR,
    OPERAND
} TokenKind;

/**
 * @brief Token data structure
 *
 * Data structure containing kind and value of a token.
 */
typedef struct Token_ {
    TokenKind kind;
    double value;
} Token;

/**
 * @brief Identifies the operator
 */
typedef enum Operator_ {
    // Nullary
    CLEAR,
    CLEAR_ALL,
    PRINT,

    // Unary
    SQUARE_ROOT,

    // Binary
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION
} Operator;

/**
 * @brief Read a line of input from stream into buffer.
 *
 * Read at most count characters from stream into buffer,
 * stopping at a newline, EOF, or when count characters
 * have been read, whichever comes first.
 *
 * @param stream stream The stream from which to read
 * @param buffer A buffer to store the characters read
 * @param count The maximum number of characters to read
 * @return 0 on success or an error code on failure.
 */
int read_line(FILE* stream, char* buffer, int count);

/**
 * @brief Split a string into tokens
 *
 * Split the str into tokens by whitespace.
 *
 * @param str The string to split
 * @param tokens An initialized queue that will be filled with tokens
 * @return 0 on success on an error code on failure.
 */
int tokenize(char* str, Queue* tokens);

/**
 * @brief Parse a string
 *
 * Parse a string into a Token, determining whether it is
 * an operator or operand.
 *
 * @param str The string to be parsed
 * @return An allocated Token struct containing type and value or NULL on error.
 */
Token* parse(const char* str);

/**
 * @brief Evaluate a token
 *
 * Evaluate a token. If the token is an operand it will be pushed onto the
 * stack, s. If it is an operator, the operands on the stack will be popped
 * and the operator applied, with the result being pushed onto the stack.
 *
 * @param token The token to evaluate
 * @param s An initialized stack
 */
void eval(const Token* token, Stack* s);

/**
 * @brief Apply a binary operator
 *
 * Apply a binary operator to two operands.
 *
 * @param operator The operator to apply
 * @param operand1 The first operand
 * @param operand2 The second operand
 * @return The result of applying the operator to the operands
 */
double apply_binary(Operator operator, double operand1, double operand2);

/**
 * @brief Apply a unary operator
 *
 * Apply a unary operator to an operand
 *
 * @param operator The operator to apply
 * @param operand The operand
 * @return The result of applying the operator to the operand
 */
double apply_unary(Operator operator, double operand);

/**
 * @brief Apply a nullary operator
 *
 * Apply a special nullary operator that doesn't operate on operands. Rather
 * than produce a result, this kind of operator is applied only for its
 * side-effects, which may involve manupalting the stack or producing output.
 *
 * @param operator The operator to apply
 * @param s The stack
 * @param output An output stream
 */
void apply_nullary(Operator operator, Stack* s, FILE* output);

/**
 * @brief Allocate and initialize a token
 *
 * Allocate memory for a token and set its kind and value.
 *
 * @param kind The kind of token
 * @param value The value of the token
 * @return An allocated and initialized Token struct or NULL on error.
 */
Token* new_token(TokenKind kind, double value);

int main(int argc, char* argv[]) {
    Stack s;
    Queue tokens;
    char buffer[128];
    char* str;
    Token* tok;

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
            free(tok);
        }

        apply_nullary(PRINT, &s, stdout);
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
    double value;

    if (!strcmp("+", str)) {
        kind = BINARY_OPERATOR;
        value = ADDITION;
    }
    else if (!strcmp("-", str)) {
        kind = BINARY_OPERATOR;
        value = SUBTRACTION;
    }
    else if (!strcmp("/", str)) {
        kind = BINARY_OPERATOR;
        value = DIVISION;
    }
    else if (!strcmp("*", str)) {
        kind = BINARY_OPERATOR;
        value = MULTIPLICATION;
    }
    else if (!strcmp("sqrt", str)) {
        kind = UNARY_OPERATOR;
        value = SQUARE_ROOT;
    }
    else if (!strcmp("ca", str)) {
        kind = NULLARY_OPERATOR;
        value = CLEAR_ALL;
    }
    else if (!strcmp("c", str)) {
        kind = NULLARY_OPERATOR;
        value = CLEAR;
    }
    else {
        kind = OPERAND;
        value = strtod(str, NULL);
    }

    return new_token(kind, value);
}

void eval(const Token* tok, Stack* s) {
    double* mem = NULL;
    double* n;

    if (tok->kind == OPERAND) {
        n = malloc(sizeof(double));
        if (!n) {
            return;
        }
        *n = tok->value;
        stack_push(s, n);
    }
    else if (tok->kind == BINARY_OPERATOR) {
        while (stack_peek(s)) {
            stack_pop(s, (void**) &n);

            if (mem) {
                *mem = apply_binary(tok->value, *n, *mem);
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
    else if (tok->kind == UNARY_OPERATOR) {
        stack_pop(s, (void**) &n);

        *n = apply_unary(tok->value, *n);

        stack_push(s, n);
    }
    else { // nullary operator
        apply_nullary(tok->value, s, stdout);
    }
}

double apply_binary(Operator operator, double operand1, double operand2) {
    switch (operator) {
        case ADDITION:
            return operand1 + operand2;

        case SUBTRACTION:
            return operand1 - operand2;

        case MULTIPLICATION:
            return operand1 * operand2;

        case DIVISION:
            return operand1 / operand2;

        default:
            return 0;
    }
}

double apply_unary(Operator operator, double operand) {
    switch (operator) {
        case SQUARE_ROOT:
            return sqrt(operand);

        default:
            return 0;
    }
}

void apply_nullary(Operator operator, Stack* s, FILE* output) {
    double* n;

    switch (operator) {
        case CLEAR:
            stack_pop(s, (void**) &n);
            break;

        case CLEAR_ALL:
            stack_destroy(s);
            break;

        case PRINT:
            n = stack_peek(s);
            if (n) {
                fprintf(output, "%f\n", *n);
            }
            break;

        default:
            break;
    }
}

Token* new_token(TokenKind kind, double value) {
    Token* tok = malloc(sizeof(Token));
    if (!tok) {
        return NULL;
    }

    tok->kind = kind;
    tok->value = value;

    return tok;
}
