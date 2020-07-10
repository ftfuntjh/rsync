#ifndef BUFFER_H
#define BUFFER_H

#include "list.h"
#include <stddef.h>

struct buffer {
    int capacity;
    int used;
    unsigned char ptr[0];
};

extern struct list * buffer_init(size_t capacity);
extern struct list * buffer_append(struct list * ptr, size_t capacity);
extern void buffer_fini(struct list * ptr);
extern void * buffer_concat(struct list * ptr, void * buf, int * len);
#endif // BUFFER_H
