#ifndef DS_H
#define DS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif

// Dynamic Array
#define DS_DA_INIT_CAPACITY 8192

typedef struct ds_dynamic_array {
    void *items;
    unsigned int item_size;
    unsigned int count;
    unsigned int capacity;
} ds_dynamic_array;

static inline void ds_dynamic_array_init(ds_dynamic_array *da, unsigned int item_size) {
    da->items = NULL;
    da->item_size = item_size;
    da->count = 0;
    da->capacity = 0;
}

static inline int ds_dynamic_array_append(ds_dynamic_array *da, const void *item) {
    if (da->count >= da->capacity) {
        unsigned int new_capacity = da->capacity == 0 ? DS_DA_INIT_CAPACITY : da->capacity * 2;
        void *new_items = realloc(da->items, new_capacity * da->item_size);
        if (!new_items) {
            fprintf(stderr, "Failed to reallocate dynamic array\n");
            exit(1);
        }
        da->items = new_items;
        da->capacity = new_capacity;
    }
    memcpy((char *)da->items + da->count * da->item_size, item, da->item_size);
    da->count++;
    return 0;
}

static inline int ds_dynamic_array_get(ds_dynamic_array *da, unsigned int index, void *item) {
    if (index >= da->count) {
        return 1;
    }
    memcpy(item, (char *)da->items + index * da->item_size, da->item_size);
    return 0;
}

// String Slice

typedef struct ds_string_slice {
    char *str;
    unsigned int len;
} ds_string_slice;

static inline int ds_string_slice_to_owned(ds_string_slice *ss, char **str) {
    *str = malloc(ss->len + 1);
    if (!*str) {
        return 1;
    }
    memcpy(*str, ss->str, ss->len);
    (*str)[ss->len] = '\0';
    return 0;
}

// IO
static inline int ds_io_read_file(const char *path, char **buffer) {
    // For demonstration: read from stdin instead of a file
    static char temp[4096];
    size_t len = fread(temp, 1, sizeof(temp), stdin);
    *buffer = malloc(len + 1);
    if (!*buffer) return -1;
    memcpy(*buffer, temp, len);
    (*buffer)[len] = '\0';
    return (int)len;
}

#endif // DS_H