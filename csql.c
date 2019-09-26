#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdbool.h>

#include "table.h"
#include "tokenizer.h"

void print_prompt() {
    printf("csql > ");
}

void print_welcome() {
    printf("Welcome to c_sql!\nI am afraid that I cannot do anything meaningful yet.\n");
}

void read_input(struct InputBuffer* b) {
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

bool is_command(struct InputBuffer* b) {
    if (b->input_length > 0 && b->buffer[0] == '.') {
        return true;
    }
    return false;
}

enum Command{
    COMMAND_QUIT,
    COMMAND_HELP,
    COMMAND_UNKNOWN,
};

enum Command match_command(struct InputBuffer* b) {
    if(strcmp(b->buffer, ".q") == 0) {
        return COMMAND_QUIT;
    }

    return COMMAND_UNKNOWN;
}

enum ExecuteResult{
    EXECUTE_OK,
    EXECUTE_FAIL,
    EXECUTE_TABLE_FULL,
};

enum ExecuteResult execute_insert(struct Statement* s, struct Table* t) {
    if(t->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    struct Row row_to_insert = s->row_to_insert;

    struct Row* row = row_slot(t, t->num_rows);

    /* copy data to location in table */
    *row = row_to_insert;
    t->num_rows += 1;

    return EXECUTE_OK;
}

void print_row(struct Row* row) {
    printf("%d\t%s\t%s\n", row->id, row->username, row->email);
}

enum ExecuteResult execute_select(struct Statement* s, struct Table* t) {
    struct Row* row;
    for(u_int32_t i=0; i < t->num_rows; i++) {
        row = row_slot(t, i);
        print_row(row);
    }

    return EXECUTE_OK;
}

enum ExecuteResult execute_statement(struct Statement* s, struct Table* t) {
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
    struct InputBuffer* input_buffer = new_input_buffer();
    struct Table* table = new_table();

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

        struct Statement statement;
        switch(parse_statement(input_buffer, &statement)) {
            case(PARSE_STATEMENT_OK):
                break;
            case(PARSE_SYNTAX_ERROR):
                printf("syntax error\n");
                continue;
            case(PARSE_STATEMENT_FAIL):
                printf("error parsing statement\n");
                continue;
            case(PARSE_STRING_TOO_LONG):
                printf("one or more of the arguments is too long\n");
                continue;
            case(PARSE_NEGATIVE_ID):
                printf("the id must be positive\n");
                continue;
            default:
                printf("unknown error\n");
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
