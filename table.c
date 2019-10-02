#include "table.h"
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

struct Table* db_open(const char* filename) {
    struct Pager* pager = pager_open(filename);
    u_int32_t row_size = ROW_SIZE;
    u_int32_t num_rows = pager->file_length/row_size;
    printf("rows in table: %u\n", num_rows);

    struct Table* t = malloc(sizeof(struct Table));
    t->pager = pager;
    t->num_rows = num_rows;

    return t;
}

struct Row* row_slot(struct Table* table, u_int32_t row_num) {
    /* get the index of the page the row is in */
    u_int32_t rows_per_page = (u_int32_t) ROWS_PER_PAGE;
    u_int32_t page_num = row_num / rows_per_page;
    printf("trying to get row: %u\n", row_num);

    struct Page* page = get_page(table->pager, page_num);
    if(page == NULL) {
        printf("got a NULL page\n");
        exit(EXIT_FAILURE);
    }

    return (struct Row*)page + row_num;
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

struct Pager* pager_open(const char* filename) {
    int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);

    if(fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd, 0, SEEK_END);

    struct Pager* pager = malloc(sizeof(struct Pager));
    pager->file_descriptor = fd;
    pager->file_length = file_length;

    // explicitely initialize all pages to NULL
    for(int i=0; i<TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }


    return pager;
}

struct Page* get_page(struct Pager* pager, u_int32_t page_num) {
    if(page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page out of bounds. %d > %d.\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    // load the page from file on cache miss
    if(pager->pages[page_num] == NULL) {
        struct Page* page = malloc(PAGE_SIZE);
        u_int32_t page_size = PAGE_SIZE;
        u_int32_t num_pages = pager->file_length/page_size;

        if(pager->file_length % PAGE_SIZE) {
            num_pages += 1;
        }

        if(page_num <= num_pages) {
            lseek(pager->file_descriptor, page_num*PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            printf("bytes read for loaded page: %li\n", bytes_read);
            if(bytes_read == -1) {
                printf("Error reading file");
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

void db_close(struct Table* table) {
    struct Pager* pager = table->pager;
    u_int32_t n_full_pages = table->num_rows / ROWS_PER_PAGE;

    for(int i=0; i<n_full_pages; i++) {
        if(pager->pages[i] == NULL) {
            continue;
        }

        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    u_int32_t rows_per_page = ROWS_PER_PAGE;
    u_int32_t n_remaining_rows = table->num_rows % rows_per_page;
    printf("Remaining rows: %u\n", n_remaining_rows);
    if(n_remaining_rows > 0) {
        u_int32_t page_num = n_full_pages;
        if(pager->pages[page_num] != NULL) {
            pager_flush(pager, page_num, n_remaining_rows*ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int result = close(pager->file_descriptor);
    if(result ==  -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    for(u_int32_t i=0; i<TABLE_MAX_PAGES; i++) {
        struct Page* p = pager->pages[i];
        if(p) {
            free(p);
            pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
};

void pager_flush(struct Pager* pager, u_int32_t page_num, u_int32_t size) {
    printf("writing %u bytes\n", size);
    if(pager->pages[page_num] == NULL) {
        printf("Tried to flush NULL page.\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->file_descriptor, page_num*PAGE_SIZE, SEEK_SET);
    if(offset == -1) {
        printf("Error seeking offset.\n");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);
    if(bytes_written == -1) {
        printf("Error writing to file.\n");
        exit(EXIT_FAILURE);
    }
}
