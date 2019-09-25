#include "table.h"

struct Table* new_table() {
    struct Table* t = malloc(sizeof(struct Table));
    t->num_rows = 0;
    for(u_int32_t i=0; i<TABLE_MAX_PAGES; i++) {
        t->pages[i] = NULL;
    }

    return t;
}

void free_table(struct Table* t) {
    for(u_int32_t i=0; i<TABLE_MAX_PAGES; i++) {
        free(t->pages[i]);
    }
    free(t);
}

struct Row* row_slot(struct Table* table, u_int32_t row_num) {
    /* get the index of the page the row is in */
    u_int32_t rows_per_page = ROWS_PER_PAGE;
    u_int32_t page_num = row_num / rows_per_page;

    void* page = table->pages[page_num];
    if(page == NULL) {
        page = table->pages[page_num] = malloc(PAGE_SIZE);
        /*TODO: init page?*/
    }

    u_int32_t row_number_within_page = row_num % rows_per_page;
    u_int32_t offset_in_bytes = row_number_within_page * ROW_SIZE;

    struct Row* row = page + offset_in_bytes;
    return row;
}

enum InsertResult insert_row(struct Table* t, struct Row r) {
    if(t->num_rows >= TABLE_MAX_ROWS) {
        return INSERT_TABLE_FULL;
    }

    struct Row* dst = row_slot(t, t->num_rows);

    /* copy data to location in table */
    *dst = r;
    t->num_rows += 1;

    return INSERT_OK;
}
