#include <stdlib.h>
#include "adc.h"
#include "stats.h"

void convertToVoltage(ADCSample *samples, unsigned int count)
{

    ADCSample *p = samples;
    ADCSample *end = samples + count;

    for (; p < end; p++)
    {

        p->voltage = ((float)p->raw_value / MAX_RAW) * VREF;
    }
}

int isOverVoltage(const ADCSample *sample)
{
    return sample->voltage > OVERVOLTAGE_LIMIT;
}

int isUnderVoltage(const ADCSample *sample)
{
    return sample->voltage < UNDERVOLTAGE_LIMIT;
}

int isSensorFault(const ADCSample *sample)
{

    return (sample->status_flags & STATUS_FAULT_BIT) != 0;
}

int isOutOfRange(const ADCSample *sample)
{
    return (sample->status_flags & STATUS_OOR_BIT) != 0;
}

void computeChannelStats(const ADCSample *samples, unsigned int count,
                          unsigned int channel_id, ChannelStats *stats)
{
    stats->channel_id = channel_id;
    stats->count = 0;
    stats->overvoltage_count = 0;
    stats->undervoltage_count = 0;
    stats->sensor_fault_count = 0;
    stats->out_of_range_count = 0;
    stats->mean_voltage = 0.0f;
    stats->min_voltage = 0.0f;
    stats->max_voltage = 0.0f;
    stats->stddev_voltage = 0.0f;


    unsigned int channel_count = 0;
    for (unsigned int i = 0; i < count; i++)
    {
        if (samples[i].channel_id == channel_id)
        {
            channel_count++;
        }
    }

    if (channel_count == 0)
    {
        return;
    }

    float *voltages = malloc(channel_count * sizeof(float));
    if (voltages == NULL)
    {
        return;
    }

    unsigned int v_index = 0;
    const ADCSample *p = samples;
    const ADCSample *end = samples + count;

    for (; p < end; p++)
    {
        if (p->channel_id != channel_id)
        {
            continue;
        }

        voltages[v_index++] = p->voltage;
        stats->count++;

        if (isOverVoltage(p))  stats->overvoltage_count++;
        if (isUnderVoltage(p)) stats->undervoltage_count++;
        if (isSensorFault(p))  stats->sensor_fault_count++;
        if (isOutOfRange(p))   stats->out_of_range_count++;
    }

    stats->mean_voltage   = calculateMean(voltages, (int)channel_count);
    stats->min_voltage    = calculateMin(voltages, (int)channel_count);
    stats->max_voltage    = calculateMax(voltages, (int)channel_count);
    stats->stddev_voltage = calculateStdDev(voltages, (int)channel_count);

    free(voltages);
}

unsigned int checkSequenceIntegrity(const ADCSample *samples, unsigned int count,
                                     SequenceGap **gaps)
{
    *gaps = NULL;

    if (count < 2)
    {
        return 0;
    }

    unsigned int capacity = 4;
    unsigned int gap_count = 0;
    SequenceGap *found = malloc(capacity * sizeof(SequenceGap));
    if (found == NULL)
    {
        return 0;
    }

    for (unsigned int i = 0; i + 1 < count; i++)
    {
        unsigned int current = samples[i].sequence_number;
        unsigned int next    = samples[i + 1].sequence_number;

        if (next != current + 1)
        {
            if (gap_count == capacity)
            {
                capacity *= 2;
                SequenceGap *bigger = realloc(found, capacity * sizeof(SequenceGap));
                if (bigger == NULL)
                {
                    *gaps = found; 
                    return gap_count;
                }
                found = bigger;
            }

            found[gap_count].record_index       = i + 1;
            found[gap_count].expected_sequence   = current + 1;
            found[gap_count].found_sequence      = next;
            gap_count++;
        }
    }

    if (gap_count == 0)
    {
        free(found);
        *gaps = NULL;
        return 0;
    }

    *gaps = found;
    return gap_count;
}