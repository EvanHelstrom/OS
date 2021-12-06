#!/bin/bash
# P3 compilation script.
# Author: Brian Law

# zero disk out and bootload
dd if=/dev/zero of=floppya.img bs=512 count=2880
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc seek=0

# set up diskmap and disk directory
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc

# proc.c compile and link
bcc -ansi -c -o proc.o proc.c

# kernel load
bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o proc.o 
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

# loading message.txt to the disk
gcc -o loadFile loadFile.c
./loadFile message.txt

# assemble library functions
as86 lib.asm -o lib.o
bcc -ansi -c -o userlib.o userlib.c

# compile and load shell program
bcc -ansi -c -o shell.o shell.c
ld86 -o shell -d shell.o userlib.o lib.o
./loadFile shell

# compile and load user programs
bcc -ansi -c -o uprog1.o uprog1.c
ld86 -o uprog1 -d uprog1.o lib.o userlib.o
./loadFile uprog1

bcc -ansi -c -o uprog2.o uprog2.c
ld86 -o uprog2 -d uprog2.o lib.o userlib.o
./loadFile uprog2



