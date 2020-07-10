#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct list * buffer_init(size_t capacity)
{
    size_t buf_len = sizeof(struct buffer) + capacity;
    struct list * p = malloc(sizeof(struct list) + buf_len);
    p->prev = p;
    p->next = p;
    memset((void *)p + sizeof(struct list), 0, buf_len);
    LIST_GET(p, struct buffer)->capacity = capacity;
    LIST_GET(p, struct buffer)->used = 0;
    return p;
}

struct list * buffer_append(struct list * ptr, size_t capacity)
{
    struct list * i = ptr;
    
    LIST_INSERT(i, buffer_init(capacity));
    return i->next;
}

void buffer_fini(struct list * ptr)
{
       struct list * i = ptr; 
       do {
           LIST_REMOVE(i);
           i = i->next;
       } while (i != ptr);
}

void * buffer_concat(struct list * ptr, void * buf, int * len)
{
    int n = 0, nbytes = *len;
    struct list * i = ptr;
    do {
        struct buffer * buf_ptr = LIST_GET(i, struct buffer);
        n += buf_ptr->used;
        printf("the current n %d.\n", n);
        if (buf_ptr->capacity != buf_ptr->used)
            break;
        i = i->next;
    } while(i != ptr);

    if (len != NULL) {
        *len = n;
    }

    if (buf && nbytes >= n) {
        int copyed = 0;
        i = ptr;
        do {
            struct buffer * buf_ptr = LIST_GET(i, struct buffer);
            memcpy(buf + copyed, buf_ptr->ptr, buf_ptr->used);
            copyed += buf_ptr->used; 
            if (buf_ptr->capacity != buf_ptr->used)
                break;
            i = i->next;
        } while(i != ptr);
    }

    return buf;
}
