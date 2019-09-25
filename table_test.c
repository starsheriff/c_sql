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

    struct Row* res = row_slot(t, t->num_rows - 1);
    assert(strcmp(res->username, "me")==0);
    assert(strcmp(res->email, "me@mail.to")==0);

    free_table(t);
    printf("...ok\n");
}

void test_insert_TABLE_MAX_ROWS() {
    struct Table* t = new_table();
    printf("...test_insert_TABLE_MAX_ROWS");

    char* username = malloc(COL_USERNAME_SIZE);
    char* email = malloc(COL_EMAIL_SIZE);
    enum InsertResult result;

    struct Row src;
    for(int i=0; i< TABLE_MAX_ROWS; i++) {

        src.id = i;

        sprintf(username, "me %d", i);
        strcpy(src.username, username);

        sprintf(email, "me_%d@mail.to", i);
        strcpy(src.email, email);

        result = insert_row(t, src);
        assert(result == INSERT_OK);
    }

    strcpy(src.username, "too many rows now");
    strcpy(src.email, "will not work");
    result = insert_row(t, src);
    assert(result == INSERT_TABLE_FULL);

    free(username);
    free(email);
    free_table(t);

    printf("...ok\n");
}

void test_using_max_col_size() {
    struct Table* t = new_table();
    printf("...test_using_max_col_size");

    char username[COL_USERNAME_SIZE+1] = {[0 ... COL_USERNAME_SIZE-1] = 'a'};
    char email[COL_EMAIL_SIZE+1] = {[0 ... COL_EMAIL_SIZE-1] = 'b'};
    enum InsertResult result;
    struct Row src;

    src.id = 1;
    strcpy(src.username, username);
    strcpy(src.email, email);

    result = insert_row(t, src);
    assert(result == INSERT_OK);

    struct Row* res = row_slot(t, t->num_rows - 1);
    assert(strcmp(res->username, username)==0);
    assert(strcmp(res->email, email)==0);


    free_table(t);
    printf("...ok\n");
}


int main(int argc, char* argv[]) {
    printf("Starting to test\n");

    test_insert_one();
    test_insert_TABLE_MAX_ROWS();
    test_using_max_col_size();
}
