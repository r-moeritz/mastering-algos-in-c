#include "../../include/stack.h"
#include "../../include/queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief The calculator
 *
 * Data structure containing calculator data.
 */
typedef struct Calculator_ {
    Stack* stack; /**< The calculator's i/o stack */
    double memory; /**< User memory */
} Calculator;

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
    TokenKind kind; /**< The kind of token */
    double value; /**< The token value */
} Token;

/**
 * @brief Identifies the operator
 */
typedef enum Operator_ {
    // Nullary
    CLEAR, /**< Pop off the top of the stack */
    CLEAR_ALL, /**< Clear the stack and user memory */
    SET_MEMORY, /**< Set user memory to the top of the stack */
    CLEAR_MEMORY, /**< Clear user memory */
    RECALL_MEMORY, /**< Push user memory onto the stack */

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
 * calculator's stack. If it is an operator, it will be applied.
 *
 * @param token The token to evaluate
 * @param calc The calculator data structure
 */
void eval(const Token* token, Calculator* calc);

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
 * side-effect of manipulating the calculator's stack and/or memory.
 *
 * @param operator The operator to apply
 * @param calc The calculator
 */
void apply_nullary(Operator operator, Calculator* calc);

/**
 * @brief Print the top stack element
 *
 * Print the top element on the stack to a stream.
 *
 * @param s The stack
 * @param stream The stream to print to
 */
void print_top(Stack* s, FILE* stream);

/**
 * @brief Initialize a calculator.
 *
 * Allocate memory for & initialize the calculator stack & zero out memory.
 */
void calculator_init(Calculator* calc);

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

int main(void) {
    Calculator calc;
    Queue tokens;
    char buffer[128];
    char* str;
    Token* tok;

    calculator_init(&calc);
    queue_init(&tokens, free);

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

            eval(tok, &calc);
            free(tok);
        }

        print_top(calc.stack, stdout);
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
    else if (!strcmp("ms", str)) {
        kind = NULLARY_OPERATOR;
        value = SET_MEMORY;
    }
    else if (!strcmp("mc", str)) {
        kind = NULLARY_OPERATOR;
        value = CLEAR_MEMORY;
    }
    else if (!strcmp("mr", str)) {
        kind = NULLARY_OPERATOR;
        value = RECALL_MEMORY;
    }
    else {
        kind = OPERAND;
        value = strtod(str, NULL);
    }

    return new_token(kind, value);
}

void eval(const Token* tok, Calculator* calc) {
    double* n1;
    double* n2 = NULL;
    Stack* s = calc->stack;

    if (tok->kind == OPERAND) {
        n1 = malloc(sizeof(double));
        if (!n1) {
            return;
        }
        *n1 = tok->value;
        stack_push(s, n1);
    }
    else if (tok->kind == BINARY_OPERATOR) {
        while (stack_peek(s)) {
            stack_pop(s, (void**) &n1);

            if (n2) {
                *n2 = apply_binary(tok->value, *n1, *n2);
                free(n1);
            }
            else {
                n2 = n1;
            }
        }

        if (n2) {
            stack_push(s, n2);
        }
    }
    else if (tok->kind == UNARY_OPERATOR) {
        stack_pop(s, (void**) &n1);

        *n1 = apply_unary(tok->value, *n1);

        stack_push(s, n1);
    }
    else { // nullary operator
        apply_nullary(tok->value, calc);
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

void apply_nullary(Operator operator, Calculator* calc) {
    double* n;
    Stack* s = calc->stack;

    switch (operator) {
        case CLEAR:
            stack_pop(s, (void**) &n);
            free(n);
            break;

        case CLEAR_ALL:
            stack_destroy(s);
            calc->memory = 0;
            break;

        case SET_MEMORY:
            n = stack_peek(s);
            if (n) {
                calc->memory = *n;
            }
            break;

        case CLEAR_MEMORY:
            calc->memory = 0;
            break;

        case RECALL_MEMORY:
            n = malloc(sizeof(double));
            if (!n) {
                break;
            }
            *n = calc->memory;
            stack_push(s, n);
            break;

        default:
            break;
    }
}

void print_top(Stack* s, FILE* stream) {
    double* n = stack_peek(s);
    if (n) {
        fprintf(stream, "%f\n", *n);
    }
}

void calculator_init(Calculator* calc) {
    calc->stack = malloc(sizeof(Stack));
    if (!calc->stack) {
        free(calc);
        return;
    }

    stack_init(calc->stack, free);

    calc->memory = 0;
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
