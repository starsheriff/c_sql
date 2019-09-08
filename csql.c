#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

InputBuffer*
new_input_buffer() {
    InputBuffer* b = (InputBuffer*)malloc(sizeof(InputBuffer));
    b->buffer = NULL;
    b->buffer_length = 0;
    b->input_length = 0;

    return b;
}

void print_prompt() {
    printf("csql > ");
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

int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = new_input_buffer();

    while(1) {
        print_prompt();
        read_input(input_buffer);

        if(strcmp(input_buffer->buffer, ".q") == 0) {
            close_buffer(input_buffer);
            exit(0);
        } else {
            printf("unrecognized command: '%s'. \n", input_buffer->buffer);
        }

    }
}
