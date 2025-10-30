#include "page.h"
#include <stddef.h> // for NULL

#define NUM_PAGES 128
#define PAGE_SIZE  (2 * 1024 * 1024) // 2 MB pages

struct ppage physical_page_array[NUM_PAGES];

struct ppage *free_physical_pages_head = NULL;

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

    for (unsigned int i = 1; i < npages && current->next != NULL; i++) {
        current = current->next;
    }

    free_physical_pages_head = current->next;
    if (free_physical_pages_head) {
        free_physical_pages_head->prev = NULL;
    }
    current->next = NULL;

    return allocd_list;
}

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

struct page_directory_entry pd[1024] __attribute__((aligned(4096)));
struct page pt[1024] __attribute__((aligned(4096)));

void *map_pages(void *vaddr, struct ppage *pglist, struct page_directory_entry *pd) {
    uint32_t va = (uint32_t)vaddr;
    struct ppage *cur = pglist;

    uint32_t pd_index = va >> 22;
    uint32_t pt_index = (va >> 12) & 0x03FF;

    if (!pd[pd_index].present) {
        pd[pd_index].frame = ((uint32_t)pt) >> 12;
        pd[pd_index].present = 1;
        pd[pd_index].rw = 1;
        pd[pd_index].user = 0;
    }

    while (cur != NULL) {
        pt[pt_index].frame = ((uint32_t)cur->physical_addr) >> 12;
        pt[pt_index].present = 1;
        pt[pt_index].rw = 1;
        pt[pt_index].user = 0;

        cur = cur->next;
        pt_index++;
        if (pt_index >= 1024) break;
    }

    return vaddr;
}
