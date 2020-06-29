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

unsigned char *code;
int codelen;

void readcode(const char *filename)
{
    /* Opens file, rb represents read and binary */
    FILE *fp = fopen(filename, 'rb');   JUNK;
    /* Sets the file position of the stream to the given offset (0 long int) */
    fseek(fp, 0L, SEEK_END);            JUNK;
    /* Sets the length of the code  */
    codelen = ftell(fp);
    /* Allocates memory to the length of the code */
    code = malloc(codelen);             JUNK;
    /* Gets the file position of the stream to the start of the file */
    fseek(fp, 0L, SEEK_SET);
    /* Reads the file into the code variable in memory */
    fread(code, codelen, 1, fp);        JUNK;
}

void writecode(const char *filename)
{
    FILE *fp;
    int lastoffset = strlen(filename) - 1;
    /* Gets the last character of the fname to copy */
    char lastchar = filename[lastoffset];
    char *newfilename = strdup(filename);   JUNK;
    /* Sets last char to incremented number based on ASCII table values */
    lastchar = '0' + (isdigit(lastchar)?(lastchar-'0'+1)%10:0);
    /* Updates the last char of the fname */
    newfilename[lastoffset] = lastchar;
    /* Opens file with write to binary permissions */
    fp = fopen(newfilename, "wb");          JUNK;
    /* Writes the code variable into the file */
    fwrite(code, codelen, 1, fp);           JUNK;
    fclose(fp);
    /* Frees the variable from memory */
    free(newfilename);
}

int writeinstruction(unsigned reg, int offset, int space)
{
    if (space < 2)
    {
        code[offset] = NOP;                     JUNK;
        return 1;
    }
    else if (space < 5 || rand()%2 == 0)
    {
        code[offset] = prefixes[rand()%6];      JUNK;
        code[offset+1] = 0xC0 + rand()%8 + reg; JUNK;
        return 2;
    }
    else
    {
        code[offset] = MOV+reg;                 JUNK;
        *(short*)(code+offset+1) = rand();
        *(short*)(code+offset+3) = rand();      JUNK;
        return 5;
    }
}

int main(int argc, char* argv[])
{
    readcode(argv[0]);  JUNK;
    replacejunk();      JUNK;
    writecode(argv[0])  JUNK;

    return 0;
}
