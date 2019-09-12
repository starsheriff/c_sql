#include "tokenizer.h"

typedef enum {
    WORD,
    COMMA,
    LBRACKET,
    RBRACKET,
} TokenType;


typedef struct {
    TokenType type;
    void* data;
} Token;

struct InputBuffer* new_input_buffer() {
    struct InputBuffer* b = (struct InputBuffer*)malloc(sizeof(struct InputBuffer));
    b->buffer = NULL;
    b->buffer_length = 0;
    b->input_length = 0;
    b->cursor = 0;

    return b;
}

void close_buffer(struct InputBuffer* b) {
    free(b->buffer);
    free(b);
}

enum ParseStatementResult
parse_statement(struct InputBuffer* b, struct Statement* s) {
    if(strncmp(b->buffer, "insert", 6) == 0) {
        s->type = STATEMENT_INSERT;
        int args_assigned = sscanf(
                b->buffer, "insert %d %s %s",
                &(s->row_to_insert.id),
                s->row_to_insert.username,
                s->row_to_insert.email);
        if (args_assigned < 3) {
            return PARSE_SYNTAX_ERROR;
        }
        return PARSE_STATEMENT_OK;
    }
    if(strncmp(b->buffer, "select", 6) == 0) {
        s->type = STATEMENT_SELECT;
        return PARSE_STATEMENT_OK;
    }

    return PARSE_STATEMENT_FAIL;
}
