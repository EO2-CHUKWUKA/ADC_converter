#ifndef ADC_ANALYSER_STATS_H
#define ADC_ANALYSER_STATS_H

#include "adc.h"



float calculateMean(
        float values[],
        int count);

float calculateMin(
        float values[],
        int count);

float calculateMax(
        float values[],
        int count);

float calculateStdDev(
        float values[],
        int count);



void calculateChannelStatistics(
        ADCSample *samples,
        uint32_t recordCount,
        uint16_t channelCount,
        ChannelStats stats[]);

#endif