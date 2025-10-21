#include "page.h"
#include <stddef.h> // for NULL

#define NUM_PAGES 128
#define PAGE_SIZE  (2 * 1024 * 1024) // 2 MB pages

// Statically allocate physical pages array
struct ppage physical_page_array[NUM_PAGES];

// Head of the free pages linked list
struct ppage *free_physical_pages_head = NULL;

// Initialize the free page list
void init_pfa_list(void) {
    for (int i = 0; i < NUM_PAGES; i++) {
        physical_page_array[i].physical_addr = (void *)(i * PAGE_SIZE);
        physical_page_array[i].next = (i < NUM_PAGES - 1) ? &physical_page_array[i + 1] : NULL;
        physical_page_array[i].prev = (i > 0) ? &physical_page_array[i - 1] : NULL;
    }
    free_physical_pages_head = &physical_page_array[0];
}

// Allocate npages from the free list
struct ppage *allocate_physical_pages(unsigned int npages) {
    if (!free_physical_pages_head || npages == 0) return NULL;

    struct ppage *allocd_list = free_physical_pages_head;
    struct ppage *current = free_physical_pages_head;

    // Move current npages forward
    for (unsigned int i = 1; i < npages && current->next != NULL; i++) {
        current = current->next;
    }

    // Detach allocated pages from free list
    free_physical_pages_head = current->next;
    if (free_physical_pages_head) {
        free_physical_pages_head->prev = NULL;
    }
    current->next = NULL;

    return allocd_list;
}

// Free a list of pages, adding them back to the free list
void free_physical_pages(struct ppage *ppage_list) {
    if (!ppage_list) return;

    struct ppage *last = ppage_list;
    while (last->next != NULL) {
        last = last->next;
    }

    // Attach the freed list to the front of the free list
    last->next = free_physical_pages_head;
    if (free_physical_pages_head) {
        free_physical_pages_head->prev = last;
    }
    free_physical_pages_head = ppage_list;
    free_physical_pages_head->prev = NULL;
}
