#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdbool.h>

#include "table.h"

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
    ssize_t cursor;
} InputBuffer;

InputBuffer*
new_input_buffer() {
    InputBuffer* b = (InputBuffer*)malloc(sizeof(InputBuffer));
    b->buffer = NULL;
    b->buffer_length = 0;
    b->input_length = 0;
    b->cursor = 0;

    return b;
}

void print_prompt() {
    printf("csql > ");
}

void print_welcome() {
    printf("Welcome to c_sql!\nI am afraid that I cannot do anything meaningful yet.\n");
}

void read_input(InputBuffer* b) {
    ssize_t bytes_read =
        getline(&b->buffer, &b->buffer_length, stdin);

    /* note: even if getline fails, we _must_ free the buffer allocated by
     * getline */
    if(bytes_read <= 0) {
        printf("error reading input");
        exit(1);
    }

    /* remove trailing new line character */
    b->input_length = bytes_read -1;
    b->buffer[bytes_read-1] = 0;
}

void close_buffer(InputBuffer* b) {
    free(b->buffer);
    free(b);
}

bool is_command(InputBuffer* b) {
    if (b->input_length > 0 && b->buffer[0] == '.') {
        return true;
    }
    return false;
}

typedef enum {
    COMMAND_QUIT,
    COMMAND_HELP,
    COMMAND_UNKNOWN,
} Command;

Command match_command(InputBuffer* b) {
    if(strcmp(b->buffer, ".q") == 0) {
        return COMMAND_QUIT;
    }

    return COMMAND_UNKNOWN;
}

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT,
} StatementType;


typedef struct {
    StatementType type;

    /* temproray place the row here, should be moved later. Not every statement
     * has a row to insert. */
    Row row_to_insert;
} Statement;

typedef enum {
    PARSE_STATEMENT_OK,
    PARSE_STATEMENT_FAIL,
    PARSE_SYNTAX_ERROR,
} ParseStatementResult;

ParseStatementResult parse_statement(InputBuffer* b, Statement* s) {
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

typedef enum {
    EXECUTE_OK,
    EXECUTE_FAIL,
    EXECUTE_TABLE_FULL,
} ExecuteResult;

ExecuteResult execute_insert(Statement* s, Table* t) {
    if(t->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    Row row_to_insert = s->row_to_insert;

    Row* row = row_slot(t, t->num_rows);

    /* copy data to location in table */
    *row = row_to_insert;
    t->num_rows += 1;

    return EXECUTE_OK;
}

void print_row(Row* row) {
    printf("%d\t%s\t%s\n", row->id, row->username, row->email);
}

ExecuteResult execute_select(Statement* s, Table* t) {
    Row* row;
    for(u_int32_t i=0; i < t->num_rows; i++) {
        row = row_slot(t, i);
        print_row(row);
    }

    return EXECUTE_OK;
}

ExecuteResult execute_statement(Statement* s, Table* t) {
    switch(s->type) {
        case STATEMENT_INSERT:
            return execute_insert(s, t);
        case STATEMENT_SELECT:
            return execute_select(s, t);
    }

    return EXECUTE_FAIL;
}

void print_help() {
    printf("type .q to exit");
}

int main(int argc, char* argv[]) {
    print_welcome();
    InputBuffer* input_buffer = new_input_buffer();
    Table* table = new_table();

    while(true) {
        print_prompt();
        read_input(input_buffer);

        if (is_command(input_buffer)) {
            switch (match_command(input_buffer)) {
            case (COMMAND_HELP):
                print_help();
            case (COMMAND_QUIT):
                exit(0);
            case (COMMAND_UNKNOWN):
                printf("unrecognized command: '%s'. \n", input_buffer->buffer);
                continue;
            default:
                continue;

            }
        }

        Statement statement;
        switch(parse_statement(input_buffer, &statement)) {
            case(PARSE_STATEMENT_OK):
                break;
            case(PARSE_SYNTAX_ERROR):
                printf("syntax error\n");
                continue;
            case(PARSE_STATEMENT_FAIL):
                printf("error parsing statement\n");
                continue;
        }

        switch (execute_statement(&statement, table)) {
            case EXECUTE_OK:
                printf("Executed.\n");
                break;
            case EXECUTE_FAIL:
                printf("Error\n");
                break;
            case EXECUTE_TABLE_FULL:
                printf("No space left in table.\n");
                break;
            default:
                printf("Error\n");
                break;
        }
    }
}
