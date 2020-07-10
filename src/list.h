#ifndef LIST_H
#define LIST_H

struct list {
    struct list * prev;
    struct list * next;
};

#define LIST_INSERT(LIST, ELEMENT) \
    do { \
        ELEMENT->prev = LIST->prev; \
        ELEMENT->next = LIST; \
        LIST->prev->next = ELEMENT; \
        LIST->prev = ELEMENT; \
    } while(0)

#define LIST_REMOVE(ELEMENT) \
    do { \
        struct list * prev = ELEMENT->prev; \
        if (prev == ELEMENT) { \
            free (ELEMENT); \
            break; \
        } \
        prev->next = ELEMENT->next; \
        ELEMENT->next->prev = prev; \
        free (ELEMENT); \
    } while(0)

#define LIST_INIT(NAME, ELE, TYPE) \
    struct list * NAME = malloc(sizeof(struct list) + sizeof(TYPE)); \
    NAME->prev = NAME; \
    NAME->next = NAME; \
    memcpy(NAME + sizeof(struct list), ELE, sizeof(TYPE))

#define LIST_INIT_ASSIGN(NAME, TYPE) \
    NAME = malloc(sizeof(struct list) + sizeof(TYPE)); \
    NAME->prev = NAME; \
    NAME->next = NAME; \
    memset((void *)NAME + sizeof(struct list), 0, sizeof(TYPE))

#define LIST_GET(NAME, TYPE) ((TYPE *)((void *)NAME + sizeof(struct list)))

#define LIST_FOREACH(NAME, ELENAME) \
    for (struct list * ELENAME = NAME; ELENAME != NAME; ELENAME = ELENAME->next)

#endif // LIST_H
