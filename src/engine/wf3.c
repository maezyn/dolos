#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

unsigned char *code;
int codelen;

srand(time(NULL));

int read_binary_file(char *filename)
{
    FILE *fp;

    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_END);
    codelen = ftell(fp);
    rewind(fp);

    code = (unsigned char *) malloc(codelen);
    fread(code, codelen, 1, fp);
    fclose(fp);
}

int write_binary_file(char *filename)
{
    FILE *fp;

    if (remove(filename) != 0)
    {
        perror("Error deleting file");
        exit(EXIT_FAILURE);
    }

    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        perror("Error writing file");
        exit(EXIT_FAILURE);
    }

    fwrite(code, codelen, 1, fp);

    fclose(fp);

    chmod(filename, 0700);
}

int exec(char *filename)
{
    printf("Reading binary file\n");
    read_binary_file(filename);


    printf("Writing binary file\n");
    write_binary_file(filename);
}

int main(int argc, char **argv)
{
    exec(argv[0]);

    exit(EXIT_SUCCESS);
}
