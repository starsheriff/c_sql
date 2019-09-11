#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdbool.h>

// temporary fields for the static table
#define COL_USERNAME_SIZE 32
#define COL_EMAIL_SIZE 255

#define TABLE_MAX_PAGES 100
#define ROW_SIZE sizeof(struct Row)
#define PAGE_SIZE 4096
#define ROWS_PER_PAGE PAGE_SIZE/ROW_SIZE
#define TABLE_MAX_ROWS ROWS_PER_PAGE*TABLE_MAX_PAGES

struct Row{
    u_int32_t id;
    char username[COL_USERNAME_SIZE];
    char email[COL_EMAIL_SIZE];
}__attribute__((packed));

struct Page{
    struct Row rows[ROWS_PER_PAGE];
};

struct Table {
    u_int32_t num_rows;
    struct Page* pages[TABLE_MAX_PAGES];
};


struct Table* new_table();
void free_table(struct Table*);
struct Row* row_slot(struct Table*, u_int32_t);
