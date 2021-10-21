# Evan Quist
#! /bin/bash

# Compile our C
bcc -ansi -c -o kernel.o kernel.c

# Assemble the Kernel.asm
as86 kernel.asm -o kernel_asm.o

# Link the kernel.o and kernel_asm.o into the executable file
ld86 -o kernel -d kernel.o kernel_asm.o

# Copy to the bootloader
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

# Place message into sector 30 for testing
dd if=message.txt of=floppya.img bs=512 conv=notrunc seek=30

