#include "table.h"



int main(int argc, char* argv[]) {
    struct Table* t = new_table();

    printf("Starting to test\n");

    struct Row insert;
    insert.id = 1;
    strcpy(insert.username, "starsheriff");
    strcpy(insert.email, "jo@starsheriff.eu");

    struct Row* cursor = row_slot(t, t->num_rows);
    *cursor = insert;

    free_table(t);
}
