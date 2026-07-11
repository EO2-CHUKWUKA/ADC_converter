//
// Created by ASUS on 12/06/2026.
//

#include <math.h>
#include "stats.h"

float calculateMean(
        float values[],
        int count)
{
    float sum = 0.0f;

    for(int i = 0;
        i < count;
        i++)
    {
        sum += values[i];
    }

    return sum / count;
}
float calculateMin(
        float values[],
        int count)
{
    float min = values[0];

    for(int i = 1;
        i < count;
        i++)
    {
        if(values[i] < min)
        {
            min = values[i];
        }
    }

    return min;
}
float calculateMax(
        float values[],
        int count)
{
    float max = values[0];

    for(int i = 1;
        i < count;
        i++)
    {
        if(values[i] > max)
        {
            max = values[i];
        }
    }

    return max;
}
float calculateStdDev(
        float values[],
        int count)
{
    float mean;
    float sumSquares = 0.0f;

    mean = calculateMean(values,
                         count);

    for(int i = 0;
        i < count;
        i++)
    {
        float difference;

        difference = values[i] - mean;

        sumSquares += difference *
                      difference;
    }

    return sqrt(sumSquares /
                count);
}