#ifndef STATS_H
#define STATS_H

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



float calculateStdDev(float values[], int count);

#endif