#include <stdint.h>
#include "page.h"
#include "fat.h"
#include "ide.h"

#define VIDEO_MEM 0xB8000
#define COLOR 0x07

extern uint32_t _end_kernel;

void loadPageDirectory(struct page_directory_entry *pd);
void enablePaging();
void identity_map_kernel_and_stack();

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void print_to_screen(const char *str) {
    unsigned short *vram = (unsigned short*)VIDEO_MEM;
    int i = 0;
    while(str[i] != '\0') {
        vram[i] = (unsigned short)str[i] | (COLOR << 8);
        i++;
    }
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

    if(fatInit() != 0) {
        print_to_screen("FAT Init Failed!");
        while(1);
    }

    struct file *f = fatOpen("TESTFILE.TXT");
    if(!f) {
        print_to_screen("File not found!");
        while(1);
    }

    char buffer[4096];
    int bytes_read = fatRead(f, buffer, f->rde.file_size);
    if(bytes_read <= 0) {
        print_to_screen("Failed to read file!");
        while(1);
    }

    if(bytes_read < 4096)
        buffer[bytes_read] = '\0';
    else
        buffer[4095] = '\0';

    print_to_screen(buffer);

    while(1) {
        uint8_t status = inb(0x64);
        if(status & 1) {
            uint8_t scancode = inb(0x60);
        }
    }
}