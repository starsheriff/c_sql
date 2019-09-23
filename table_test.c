#include "table.h"
#include <assert.h>


void test_insert_one() {
    struct Table* t = new_table();
    printf("...test_insert_one");

    struct Row src;
    src.id = 1;
    strcpy(src.username, "me");
    strcpy(src.email, "me@mail.to");

    enum InsertResult result = insert_row(t, src);

    assert(result == INSERT_OK);
    printf("...ok\n");

    free_table(t);
}


int main(int argc, char* argv[]) {
    printf("Starting to test\n");

    test_insert_one();
}
