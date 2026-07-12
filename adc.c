//
// adc.c
//

#include <stdlib.h>

#include "adc.h"
#include "stats.h"

void convertVoltagesArray(
        ADCSample *samples,
        uint32_t count)
{
    for(uint32_t i = 0;
        i < count;
        i++)
    {
        samples[i].voltage =
                (samples[i].raw_value / 4095.0f)
                * VREF;
    }
}

void convertVoltages(
        ADCSample *samples,
        uint32_t count)
{
    ADCSample *ptr =
        samples;

    for(uint32_t i = 0;
        i < count;
        i++)
    {
        ptr->voltage =
            (ptr->raw_value /
            4095.0f) *
            VREF;

        ptr++;
    }
}

int isOverVoltage(
        const ADCSample *sample)
{
    return
        sample->voltage
        > OVERVOLTAGE_LIMIT;
}

int isUnderVoltage(
        const ADCSample *sample)
{
    return
        sample->voltage
        < UNDERVOLTAGE_LIMIT;
}

int isSensorFault(
        const ADCSample *sample)
{
    return
        (sample->status_flags
        & STATUS_FAULT_BIT)
        != 0;
}

int isOutOfRange(
        const ADCSample *sample)
{
    return
        (sample->status_flags
        & STATUS_OOR_BIT)
        != 0;
}

void computeChannelStats(
        const ADCSample *samples,
        uint32_t count,
        uint32_t channel_id,
        ChannelStats *stats)
{
    stats->channel_id =
        channel_id;

    stats->count =
        0;

    stats->overvoltage_count =
        0;

    stats->undervoltage_count =
        0;

    stats->sensor_fault_count =
        0;

    stats->out_of_range_count =
        0;

    stats->mean_voltage =
        0.0f;

    stats->min_voltage =
        0.0f;

    stats->max_voltage =
        0.0f;

    stats->stddev_voltage =
        0.0f;

    uint32_t channel_count =
        0;

    for(uint32_t i = 0;
        i < count;
        i++)
    {
        if(samples[i].channel_id
            == channel_id)
        {
            channel_count++;
        }
    }

    if(channel_count == 0)
    {
        return;
    }

    float *voltages =
        malloc(channel_count * sizeof(float));

    if(voltages == NULL)
    {
        return;
    }

    uint32_t v_index =
        0;

    const ADCSample *ptr =
        samples;

    for(uint32_t i = 0;
        i < count;
        i++)
    {
        if(ptr->channel_id
            != channel_id)
        {
            ptr++;
            continue;
        }

        voltages[v_index] =
            ptr->voltage;

        v_index++;

        stats->count++;

        if(isOverVoltage(ptr))
        {
            stats->overvoltage_count++;
        }

        if(isUnderVoltage(ptr))
        {
            stats->undervoltage_count++;
        }

        if(isSensorFault(ptr))
        {
            stats->sensor_fault_count++;
        }

        if(isOutOfRange(ptr))
        {
            stats->out_of_range_count++;
        }

        ptr++;
    }

    stats->mean_voltage =
        meanOf(voltages, channel_count);

    stats->min_voltage =
        minOf(voltages, channel_count);

    stats->max_voltage =
        maxOf(voltages, channel_count);

    stats->stddev_voltage =
        stddevOf(voltages, channel_count, stats->mean_voltage);

    free(voltages);
}

uint32_t checkSequenceIntegrity(
        const ADCSample *samples,
        uint32_t count,
        SequenceGap **gaps)
{
    *gaps =
        NULL;

    if(count < 2)
    {
        return 0;
    }

    uint32_t capacity =
        4;

    uint32_t gap_count =
        0;

    SequenceGap *found =
        malloc(capacity * sizeof(SequenceGap));

    if(found == NULL)
    {
        return 0;
    }

    for(uint32_t i = 0;
        i + 1 < count;
        i++)
    {
        uint32_t current =
            samples[i].sequence_number;

        uint32_t next =
            samples[i + 1].sequence_number;

        if(next != current + 1)
        {
            if(gap_count == capacity)
            {
                capacity =
                    capacity * 2;

                SequenceGap *bigger =
                    realloc(found, capacity * sizeof(SequenceGap));

                if(bigger == NULL)
                {
                    *gaps =
                        found;

                    return gap_count;
                }

                found =
                    bigger;
            }

            found[gap_count].record_index =
                i + 1;

            found[gap_count].expected_sequence =
                current + 1;

            found[gap_count].found_sequence =
                next;

            gap_count++;
        }
    }

    if(gap_count == 0)
    {
        free(found);

        *gaps =
            NULL;

        return 0;
    }

    *gaps =
        found;

    return gap_count;
}