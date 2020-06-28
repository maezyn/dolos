#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PUSH 0x50
#define POP 0x58
#define MOV 0xB8
#define NOP 0x90

#define ADD 0x01
#define AND 0x21
#define XOR 0x31
#define OR 0x09
#define SBB 0x19
#define SUB 0x29

#define JUNK asm __volatile__(".byte 0x50, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x58\n");
#define JUNKLEN 8

int main(int argc, char* argv[])
{
    readcode(argv[0]);  JUNK;
    replacejunk();      JUNK;
    writecode(argv[0])  JUNK;

    return 0;

