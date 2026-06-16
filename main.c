#include <stdio.h>
#include <stdlib.h>

#include "adc.h"
#include "io.h"

int main(void)
{
    FILE *fp;

    ADCHeader header;

       printf("Opening: ../adc_sensor_log.bin\n");
       fp = openBinaryFile("../adc_sensor_log.bin");

    if(fp == NULL)
    {
        return 1;
    }


    if(!readHeader(fp, &header))
    {
        printf("Header read failed\n");

        fclose(fp);

        return 1;
    }

    printf("Magic Number : 0x%X\n",
           header.magic);

    printf("Version      : %u\n",
           header.version);

    printf("Channels     : %u\n",
           header.channel_count);

    printf("Records      : %u\n",
           header.record_count);

    printf("Sample Rate  : %u Hz\n",
           header.sample_rate);

    fclose(fp);

    return 0;
}