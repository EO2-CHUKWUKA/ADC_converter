#include <stdio.h>
#include <stdlib.h>

#include "adc.h"
#include "io.h"
#include "stats.h"


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <adc_sensor_log.bin>\n", argv[0]);
        return 1;
    }

    FILE *fp;
    ADCHeader header;
    ADCSample *samples;
    fp = openBinaryFile(argv[1]);

    if (fp == NULL)
    {
        return 1;
    }

    if (!readHeader(fp, &header))
    {
        fclose(fp);
        return 1;
    }

    printf("Magic Number: 0x%X\n", header.magic);
    printf("Version: %d\n", header.version);
    printf("Channels: %d\n", header.channel_count);
    printf("Records: %d\n", header.record_count);
    printf("Sample Rate: %d Hz\n\n", header.sample_rate);

    samples = malloc(header.record_count * sizeof(ADCSample));

    if (samples == NULL)
    {
        printf("Memory allocation failed\n");
        fclose(fp);
        return 1;
    }

    if (!loadRecords(fp, samples, header.record_count))
    {
        free(samples);
        fclose(fp);
        return 1;
    }

    fclose(fp);

    convertToVoltage(samples, header.record_count);

    ChannelStats channelStats[NUM_CHANNELS];

    for (unsigned int channel = 0; channel < header.channel_count; channel++)
    {
        computeChannelStats(samples, header.record_count, channel, &channelStats[channel]);

        printf("\nChannel %u\n", channel);
        printf("----------------\n");

        if (channelStats[channel].count > 0)
        {
            printf("Mean Voltage: %.3f V\n", channelStats[channel].mean_voltage);
            printf("Minimum Voltage: %.3f V\n", channelStats[channel].min_voltage);
            printf("Maximum Voltage: %.3f V\n", channelStats[channel].max_voltage);
            printf("Std Deviation: %.3f V\n", channelStats[channel].stddev_voltage);

            printf("Overvoltage faults: %u\n", channelStats[channel].overvoltage_count);
            printf("Undervoltage faults: %u\n", channelStats[channel].undervoltage_count);
            printf("Sensor fault flags: %u\n", channelStats[channel].sensor_fault_count);
            printf("Out-of-range flags: %u\n", channelStats[channel].out_of_range_count);
        }
        else
        {
            printf("No samples found\n");
        }
    }

    SequenceGap *gaps = NULL;
    unsigned int gapCount = checkSequenceIntegrity(samples, header.record_count, &gaps);
    printf("\nSequence integrity: %u gap(s) found\n", gapCount);

    if (!writeResults("results.txt", channelStats, header.channel_count,
                       gaps, gapCount, header.record_count))
    {
        printf("Failed to write results.txt\n");
        free(gaps);
        free(samples);
        return 1;
    }

    printf("Results written to results.txt\n");

    free(gaps);
    free(samples);

    return 0;
}