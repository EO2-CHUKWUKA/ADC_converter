//
// Created by EO2-CHUKWUKA on 12/06/2026.
//

#ifndef ADC_ANALYSER_STATS_H
#define ADC_ANALYSER_STATS_H

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

#endif //ADC_ANALYSER_STATS_H
