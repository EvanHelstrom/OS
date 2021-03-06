# Evan Quist
#! /bin/bash
bcc -ansi -c -o kernel.o kernel.c
# Compile our C
as86 kernel.asm -o kernel_asm.o
# Assemble the Kernel.asm
ld86 -o kernel -d kernel.o kernel_asm.o
# Link the kernel.o and kernel_asm.o into the executable file
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
# Copy to the bootloader

