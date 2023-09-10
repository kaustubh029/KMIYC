// Encryption and Decryption header file
#ifndef ENCRDEC_H
#define ENCRDEC_H

#include <stdio.h>

FILE *filepointer;
FILE *tempfilepointer;

int key = 73;

int encrypt(char filename[])
{
    filepointer = fopen(filename, "r");
    if (filepointer == NULL)
        return 0;
    tempfilepointer = fopen("temp.txt", "w");
    if (tempfilepointer == NULL)
        return 0;
    char ch = fgetc(filepointer);
    while (ch != EOF)
    {
        ch = ch + key;
        fputc(ch, tempfilepointer);
        ch = fgetc(filepointer);
    }
    fclose(filepointer);
    fclose(tempfilepointer);

    filepointer = fopen(filename, "w");
    if (filepointer == NULL)
        return 0;
    tempfilepointer = fopen("temp.txt", "r");
    if (tempfilepointer == NULL)
        return 0;
    ch = fgetc(tempfilepointer);
    while (ch != EOF)
    {
        ch = fputc(ch, filepointer);
        ch = fgetc(tempfilepointer);
    }
    fclose(filepointer);
    fclose(tempfilepointer);
    return 0;
}

int decrypt(char filename[])
{
    filepointer = fopen(filename, "w");
    if (filepointer == NULL)
        return 0;
    tempfilepointer = fopen("temp.txt", "r");
    if (tempfilepointer == NULL)
        return 0;
    char ch = fgetc(tempfilepointer);
    while (ch != EOF)
    {
        ch = ch - key;
        fputc(ch, filepointer);
        ch = fgetc(tempfilepointer);
    }
    fclose(filepointer);
    fclose(tempfilepointer);
    return 0;
}

#endif