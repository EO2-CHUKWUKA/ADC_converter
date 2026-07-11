#include <stdio.h>
#include <stdlib.h>

#include "adc.h"
#include "io.h"
#include "stats.h"

int main(void)
{
    FILE *fp;

    ADCHeader header;
    ADCSample *samples;


    // Open binary file
    fp = openBinaryFile("../adc_sensor_log.bin");

    if(fp == NULL)
    {
        printf("File opening failed\n");
        return 1;
    }


    // Read header
    if(!readHeader(fp, &header))
    {
        printf("Header read failed\n");
        fclose(fp);
        return 1;
    }


    // Display header information
    printf("Magic Number: 0x%X\n", header.magic);
    printf("Version: %d\n", header.version);
    printf("Channels: %d\n", header.channel_count);
    printf("Records: %d\n", header.record_count);
    printf("Sample Rate: %d Hz\n\n", header.sample_rate);


    fseek(fp, 0, SEEK_END);

    long fileSize = ftell(fp);

    printf("File size: %ld bytes\n", fileSize);

    printf("Header size: %lu bytes\n", sizeof(ADCHeader));

    printf("Sample size: %lu bytes\n", sizeof(ADCSample));

    printf("Expected data size: %lu bytes\n",
           sizeof(ADCHeader) + (header.record_count * sizeof(ADCSample)));


    // Return file pointer to start of records
    fseek(fp, sizeof(ADCHeader), SEEK_SET);

    // Allocate memory for samples
    samples = malloc(header.record_count * sizeof(ADCSample));


    if(samples == NULL)
    {
        printf("Memory allocation failed\n");
        fclose(fp);
        return 1;
    }



    // Load ADC records
    if(!loadRecords(fp, samples, header.record_count))
    {
        printf("Record loading failed\n");

        free(samples);
        fclose(fp);

        return 1;
    }


    fclose(fp);



    // Convert raw ADC values to voltage
    for(unsigned int i = 0; i < header.record_count; i++)
    {
        samples[i].voltage =
        (samples[i].raw_value / 4095.0f) * VREF;
    }



    // Analyse each channel
    for(unsigned int channel = 0; channel < header.channel_count; channel++)
    {
        printf("\nChannel %d\n", channel);
        printf("----------------\n");


        float sum = 0;
        float min = 100;
        float max = 0;

        int overVoltage = 0;
        int underVoltage = 0;


        int count = 0;


        for(unsigned int i = 0; i < header.record_count; i++)
        {

            if(samples[i].channel_id == channel)
            {

                float voltage = samples[i].voltage;


                sum += voltage;


                if(voltage < min)
                    min = voltage;


                if(voltage > max)
                    max = voltage;



                if(voltage > 3.0f)
                    overVoltage++;


                if(voltage < 0.3f)
                    underVoltage++;


                count++;
            }
        }



        if(count > 0)
        {
            printf("Mean Voltage: %.3f V\n", sum / count);
            printf("Minimum Voltage: %.3f V\n", min);
            printf("Maximum Voltage: %.3f V\n", max);

            printf("Overvoltage faults: %d\n", overVoltage);
            printf("Undervoltage faults: %d\n", underVoltage);
        }
        else
        {
            printf("No samples found\n");
        }

    }



    free(samples);


    return 0;
}