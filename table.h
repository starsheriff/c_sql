#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdbool.h>

// temporary fields for the static table
#define COL_USERNAME_SIZE 32
#define COL_EMAIL_SIZE 255

#define ROW_SIZE sizeof(struct Row)
#define PAGE_SIZE 4096
#define ROWS_PER_PAGE PAGE_SIZE/ROW_SIZE
#define TABLE_MAX_PAGES 100
#define TABLE_MAX_ROWS ROWS_PER_PAGE*TABLE_MAX_PAGES

struct Row{
    u_int32_t id;
    char username[COL_USERNAME_SIZE+1];
    char email[COL_EMAIL_SIZE+1];
}__attribute__((packed));

struct Page{
    struct Row rows[ROWS_PER_PAGE];
};

struct Table {
    u_int32_t num_rows;
    struct Pager* pager;
};

enum InsertResult{
    INSERT_OK,
    INSERT_TABLE_FULL,
};

struct Table* db_open(const char* filename);
struct Row* row_slot(struct Table*, u_int32_t);

enum InsertResult insert_row(struct Table*, struct Row);

struct Pager {
    int file_descriptor;
    u_int32_t file_length;
    struct Page* pages[TABLE_MAX_PAGES];
};


struct Pager* pager_open(const char* filename);
struct Page* get_page(struct Pager* pager, u_int32_t page_num);
void db_close(struct Table* table);
void pager_flush(struct Pager* pager, u_int32_t page_num, u_int32_t n_bytes);
