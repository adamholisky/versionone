ODIR = ../build
SDIR = ./

export _OBJS_ASM = boot.o interrupts_asm.o
export _OBJS_LIB_C = ctype.o errno.o math_exp.o math_misc.o math_pow.o math_trigo.o stdio.o stdlib.o string.o stdlib.o uchar.o wchar.o
export _OBJS_GUI = console.o window.o container.o control.o
export _OBJS_C = ext2.o fat.o ata.o serial.o multiboot.o memory.o base.o interrupts.o keyboard.o processes.o command.o gui.o term.o vga.o timer.o kernel.o 
export _OBJS_HELLO = hello_container.o
export _OBJS = $(_OBJS_ASM) $(_OBJS_C) $(_OBJS_LIB_C) $(_OBJS_GUI) $(_OBJS_HELLO)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

export ENABLE_GRAPHICS=1

all:
	make -C asm
	make -C hello-world
	make -C c
	make -C libc
	make -C gui
	i686-elf-g++ -T linker.ld -o ../build/versionone.bin -ffreestanding -fno-rtti  -O2 -nostdlib -z muldefs $(OBJS)

clean:
	make -C asm clean
	make -C hello-world clean
	make -C c clean
	make -C libc clean
	make -C gui clean