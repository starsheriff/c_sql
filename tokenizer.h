#pragma once
#include "table.h"

struct InputBuffer{
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
    ssize_t cursor;
};

enum StatementType{
    STATEMENT_INSERT,
    STATEMENT_SELECT,
};

enum ParseStatementResult{
    PARSE_STATEMENT_OK,
    PARSE_STATEMENT_FAIL,
    PARSE_SYNTAX_ERROR,
    PARSE_STRING_TOO_LONG,
};

struct Statement{
    enum StatementType type;

    /* temproray place the row here, should be moved later. Not every statement
     * has a row to insert. */
    struct Row row_to_insert;
};


struct InputBuffer* new_input_buffer();
void close_buffer(struct InputBuffer* b);

enum ParseStatementResult
parse_statement(struct InputBuffer* b, struct Statement* s);

enum ParseStatementResult 
prepare_insert_statement(struct InputBuffer* b, struct Statement* s);
