#include "fat.h"
#include "ide.h"
#include <stdint.h>
#include <stddef.h>

struct boot_sector bs;
char fat_table[CLUSTER_SIZE];
char root_dir_region[16384];

static int my_strncmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        if (s1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

int fatInit() {
    if (ata_lba_read(0, (unsigned char*)&bs, 1) < 0) {
        return -1;
    }

    if (bs.boot_signature != 0xAA55) {
        return -1;
    }

    if (my_strncmp(bs.fs_type, "FAT", 3) != 0) {
        return -1;
    }

    return 0;
}

struct file* fatOpen(const char* filename) {
    (void)filename;
    return 0;
}

int fatRead(struct file* f, char* buffer, int size) {
    (void)f;
    (void)buffer;
    (void)size;
    return 0;
}
