//
// Created by  on 12/06/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include "io.h"

FILE *openBinaryFile(const char * filename)
{
    FILE *fp = fopen(filename, "rb");

    if(fp == NULL)
    {
        perror("Error opening file");
    }

    return fp;
}
int readHeader(FILE *fp,
               ADCHeader *header)
{
    return fread(header,
                 sizeof(ADCHeader),
                 1,
                 fp);
}
int loadRecords(FILE *fp,
                ADCSample *samples,
                uint32_t count)
{
    ADCSample *ptr = samples;

    for(uint32_t i = 0; i < count; i++)
    {
        if(fread(ptr,
                 sizeof(ADCSample),
                 1,
                 fp) != 1)
        {
            return 0;
        }

        ptr++;
    }

    return 1;
}

