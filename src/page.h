#ifndef PAGE_H
#define PAGE_H

struct ppage {
    struct ppage *next;
    struct ppage *prev;
    void *physical_addr;
};

// Initialize the free page list
void init_pfa_list(void);

// Allocate npages from the free list; returns head of allocated list
struct ppage *allocate_physical_pages(unsigned int npages);

// Free a list of pages, adding them back to the free list
void free_physical_pages(struct ppage *ppage_list);

#endif // PAGE_H
