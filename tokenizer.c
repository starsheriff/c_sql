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
        return prepare_insert_statement(b, s);
    }
    if(strncmp(b->buffer, "select", 6) == 0) {
        s->type = STATEMENT_SELECT;
        return PARSE_STATEMENT_OK;
    }

    return PARSE_STATEMENT_FAIL;
}

enum ParseStatementResult
prepare_insert_statement(struct InputBuffer* b, struct Statement* s) {
    s->type = STATEMENT_INSERT;

    char* keyword = strtok(b->buffer, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if(keyword==NULL || id_string==NULL || username==NULL || email==NULL) {
        return PARSE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if(strlen(username) > COL_USERNAME_SIZE) {
        return PARSE_STRING_TOO_LONG;
    }
    if(strlen(email) > COL_EMAIL_SIZE) {
        return PARSE_STRING_TOO_LONG;
    }

    s->row_to_insert.id = id;
    strcpy(s->row_to_insert.username, username);
    strcpy(s->row_to_insert.email, email);

    return PARSE_STATEMENT_OK;
}
