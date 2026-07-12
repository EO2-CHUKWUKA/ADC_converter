//
// Created by ASUS on 12/06/2026.
//

#include <stdio.h>
#include <stdlib.h>

#include "io.h"

/*-------------------------------------------------
  Open the binary file
--------------------------------------------------*/
FILE *openBinaryFile(const char *filename)
{
    FILE *fp;

    fp = fopen(filename, "rb");

    if(fp == NULL)
    {
        perror("Error opening binary file");
    }

    return fp;
}

/*-------------------------------------------------
  Read the file header
--------------------------------------------------*/
int readHeader(FILE *fp,
               ADCHeader *header)
{
    if(fp == NULL || header == NULL)
    {
        return 0;
    }

    if(fread(header,
             sizeof(ADCHeader),
             1,
             fp) != 1)
    {
        return 0;
    }

    if(header->magic != ADC_MAGIC)
    {
        printf("Invalid magic number.\n");
        return 0;
    }

    return 1;
}

/*-------------------------------------------------
  Load every ADC record
--------------------------------------------------*/
int loadRecords(FILE *fp,
                ADCSample *samples,
                uint32_t count)
{
    if(fp == NULL || samples == NULL)
    {
        return 0;
    }

    for(uint32_t i = 0; i < count; i++)
    {
        if(fread(&samples[i],
                 sizeof(ADCSample),
                 1,
                 fp) != 1)
        {
            printf("Read failed at record %u\n", i);
            return 0;
        }
    }

    return 1;
}