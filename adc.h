#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define VREF                3.3f     /* reference voltage, volts   */
#define MAX_RAW             4095.0f
#define NUM_CHANNELS        4

#define OVERVOLTAGE_LIMIT   3.0f
#define UNDERVOLTAGE_LIMIT  0.3f

#define STATUS_FAULT_BIT    0x01u    /* bit 0: sensor fault        */
#define STATUS_OOR_BIT      0x02u    /* bit 1: out-of-range        */


typedef struct
{
    float          timestamp;
    unsigned char  channel_id;
    unsigned short raw_value;
    float          voltage;
    short          temperature;
    unsigned char  status_flags;
    unsigned int   sequence_number;
} ADCSample;

typedef struct
{
    unsigned int channel_id;
    unsigned int count;

    float mean_voltage;
    float min_voltage;
    float max_voltage;
    float stddev_voltage;

    unsigned int overvoltage_count;
    unsigned int undervoltage_count;
    unsigned int sensor_fault_count;
    unsigned int out_of_range_count;
} ChannelStats;

typedef struct
{
    unsigned int record_index;
    unsigned int expected_sequence;
    unsigned int found_sequence;
} SequenceGap;


void convertToVoltage(ADCSample *samples, unsigned int count);


int isOverVoltage(const ADCSample *sample);
int isUnderVoltage(const ADCSample *sample);
int isSensorFault(const ADCSample *sample);
int isOutOfRange(const ADCSample *sample);


void computeChannelStats(const ADCSample *samples, unsigned int count,
                          unsigned int channel_id, ChannelStats *stats);


unsigned int checkSequenceIntegrity(const ADCSample *samples, unsigned int count,
                                     SequenceGap **gaps);

#endif