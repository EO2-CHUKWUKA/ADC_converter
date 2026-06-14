//
// Created by EO2-CHUKWUKA on 12/06/2026.
//

#ifndef ADC_ANALYSER_IO_H
#define ADC_ANALYSER_IO_H

#include <stdio.h>
#include "adc.h"

FILE *openBinaryFile(
        const char *adc_sensor_log.bin);
int readHeader(
        FILE *fp,
        ADCHeader *header);

int loadRecords(
        FILE *fp,
        ADCSample *samples,
        uint32_t count);

#endif //ADC_ANALYSER_IO_H
