#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdbool.h>

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
} Statement;

typedef enum {
    PARSE_STATEMENT_OK,
    PARSE_STATEMENT_FAIL,
} ParseStatementResult;

ParseStatementResult parse_statement(InputBuffer* b, Statement* s) {
    if(strncmp(b->buffer, "insert", 6) == 0) {
        s->type = STATEMENT_INSERT;
        return PARSE_STATEMENT_OK;
    }
    if(strncmp(b->buffer, "select", 6) == 0) {
        s->type = STATEMENT_SELECT;
        return PARSE_STATEMENT_OK;
    }

    return PARSE_STATEMENT_FAIL;
}

void execute_statement(Statement* s) {
    switch(s->type) {
        case STATEMENT_INSERT:
            printf("received an insert statement\n");
            break;
        case STATEMENT_SELECT:
            printf("received an select statement\n");
            break;
    }
}

void print_help() {
    printf("type .q to exit");
}

int main(int argc, char* argv[]) {
    print_welcome();
    InputBuffer* input_buffer = new_input_buffer();

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
            case(PARSE_STATEMENT_FAIL):
                printf("error parsing statement\n");
                continue;
        }

        execute_statement(&statement);

    }
}
