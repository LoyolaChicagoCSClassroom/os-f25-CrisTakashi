#include <stdint.h>
#include <stddef.h>
#include "page.h"  // Added for page frame allocator

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

void loadPageDirectory(struct page_directory_entry *pd) {
    asm volatile("mov %0, %%cr3" :: "r"(pd));
}

void enablePaging() {
    asm volatile(
        "mov %%cr0, %%eax\n"
        "or $0x80000001, %%eax\n"
        "mov %%eax, %%cr0"
        :::"eax"
    );
}

extern uint32_t _end_kernel;

void identity_map_kernel_and_stack() {
    uint32_t *vaddr;
    uint32_t page_size = 0x1000; // 4 KB

    // Map kernel
    for (vaddr = (uint32_t*)0x100000; vaddr < &_end_kernel; vaddr += page_size) {
        struct ppage tmp;
        tmp.next = NULL;
        tmp.physical_addr = (void*)vaddr;
        map_pages(vaddr, &tmp, pd);
    }

    // Map stack
    uint32_t esp;
    asm("mov %%esp,%0" : "=r"(esp));
    for (vaddr = (uint32_t*)(esp & ~0xFFF); vaddr > (uint32_t*)(esp - 0x10000); vaddr -= page_size) {
        struct ppage tmp;
        tmp.next = NULL;
        tmp.physical_addr = (void*)vaddr;
        map_pages(vaddr, &tmp, pd);
    }

    // Map video buffer
    struct ppage video;
    video.next = NULL;
    video.physical_addr = (void*)0xB8000;
    map_pages((void*)0xB8000, &video, pd);
}

const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {
    MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12
};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void main() {
    init_pfa_list();

    struct ppage *alloc1 = allocate_physical_pages(3);

    struct ppage *alloc2 = allocate_physical_pages(2);

    free_physical_pages(alloc1);

    struct ppage *alloc3 = allocate_physical_pages(1);

    identity_map_kernel_and_stack();
    loadPageDirectory(pd);
    enablePaging();

    unsigned short *vram = (unsigned short*)0xb8000;
    const unsigned char color = 7;

    while(1) {
        uint8_t status = inb(0x64);

        if(status & 1) {
            uint8_t scancode = inb(0x60);
        }
    }
}
