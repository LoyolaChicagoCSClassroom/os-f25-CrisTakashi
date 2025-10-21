#include <stdint.h>
#include "page.h"  // Added for page frame allocator

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

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

    unsigned short *vram = (unsigned short*)0xb8000; // Base address of video mem
    const unsigned char color = 7; // gray text on black background

    while(1) {
        uint8_t status = inb(0x64);

        if(status & 1) {
            uint8_t scancode = inb(0x60);
        }
    }
}
