//
// Created by EO2-Chukwuka on 12/06/2026.
//

#ifndef ADC_ANALYSER_ADC_H
#define ADC_ANALYSER_ADC_H

#include<stdint.h>

#define  ADC_MAGIC 0xADC1BEEF
#define VREF 3.3f

typedef struct __attribute__ ((packed))
{
    uint32_t magic;
    uint16_t version;
    uint16_t channel_count;
    uint32_t record_count;
    uint32_t sample_rate;
    uint8_t reserved[8];
} ADCHeader;
typedef struct __attribute__((packed))
{
    float timestamp;
    uint8_t channel_id;
    uint16_t raw_value;
    int16_t temperature;
    uint8_t status_flags;
    uint32_t sequence_number;
    uint8_t reserved[2];

    float voltage;
} ADCSample;

typedef struct
{
    float mean ;
    float min;
    float max;
    float stddev;

    int over_voltage;
    int under_voltage;
    int sensor_fault;

} ChannelStats;
// now including the adc.c functions into my header code//

void convertVoltages(
        ADCSample *samples,
        uint32_t count
        );

int checkSequence(
        ADCSample *samples,
        uint32_t count
        );

void detectFaults(
        ADCSample *samples,
        uint32_t count,
        ChannelStats stats[]
        );




#endif //ADC_ANALYSER_ADC_H


