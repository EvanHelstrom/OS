# Evan Quist
#! /bin/bash

# Compile our C
bcc -ansi -c -o kernel.o kernel.c

# Assemble the Kernel.asm
as86 kernel.asm -o kernel_asm.o

# Link the kernel.o and kernel_asm.o into the executable file
ld86 -o kernel -d kernel.o kernel_asm.o

# Put the bootloader on the drive
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc seek=0 

# Copy to the bootloader
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

# Place disk map and directory in the file system
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc

# Compile LoadFile for use in next line
gcc -o loadFile loadFile.c

# Load the message into the file system
./loadFile message.txt

# Compile Link and Load User program 1
as86 lib.asm -o lib.o
bcc -ansi -c -o uprog1.o uprog1.c
ld86 -o uprog1 -d uprog1.o lib.o
./loadFile uprog1

# Compile Link and Load User program 2
bcc -ansi -c -o uprog2.o uprog2.c
ld86 -o uprog2 -d uprog2.o lib.o
./loadFile uprog2

# Compile the userlib.c
bcc -ansi -c -o userlib.o userlib.c

# Compile Link and Load the Shell

bcc -ansi -c -o shell.o shell.c
ld86 -o shell -d shell.o lib.o userlib.o 
./loadFile shell



