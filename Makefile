CC=i686-linux-gnu-gcc
AS=nasm
LD=i686-linux-gnu-ld
CFLAGS=-ffreestanding -mgeneral-regs-only -mno-mmx -m32 -march=i386 -fno-pie -fno-stack-protector -Wall -g3
ASFLAGS=-f elf32
LDFLAGS=-T linker.ld

OBJDIR=obj
OBJS=$(OBJDIR)/kernel_main.o $(OBJDIR)/page.o $(OBJDIR)/ide.o $(OBJDIR)/paging.o $(OBJDIR)/fat.o

all: kernel rootfs.img

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/ide.o: src/ide.s | $(OBJDIR)
	$(AS) $(ASFLAGS) $< -o $@

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

rootfs.img:
	dd if=/dev/zero of=rootfs.img bs=512 count=32768

clean:
	rm -rf $(OBJDIR) kernel rootfs.img
