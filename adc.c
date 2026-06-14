//
// Created by ASUS on 12/06/2026.
//

#include "adc.h"
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