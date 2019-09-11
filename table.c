#include "table.h"

Table* new_table() {
    Table* t = malloc(sizeof(Table));
    t->num_rows = 0;
    for(u_int32_t i=0; i<TABLE_MAX_PAGES; i++) {
        t->pages[i] = NULL;
    }

    return t;
}

void free_table(Table* t) {
    for(u_int32_t i=0; i<TABLE_MAX_PAGES; i++) {
        free(t->pages[i]);
    }
    free(t);
}

Row* row_slot(Table* table, u_int32_t row_num) {
    /* get the index of the page the row is in */
    u_int32_t page_num = row_num / ROWS_PER_PAGE;

    void* page = table->pages[page_num];
    if(page == NULL) {
        page = table->pages[page_num] = malloc(PAGE_SIZE);
        /*TODO: init page?*/
    }

    u_int32_t row_number_within_page = row_num % ROWS_PER_PAGE;
    u_int32_t offset_in_bytes = row_number_within_page * ROW_SIZE;

    Row* row = page + offset_in_bytes;
    return row;
}

