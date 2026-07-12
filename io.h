#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdint.h>
#include "adc.h"

#define EXPECTED_MAGIC          0xADC1BEEFu
#define EXPECTED_VERSION        1
#define EXPECTED_CHANNEL_COUNT  4
#define EXPECTED_SAMPLE_RATE    1000

typedef struct __attribute__((packed))
{
    uint32_t magic;
    uint16_t version;
    uint16_t channel_count;
    uint32_t record_count;
    uint32_t sample_rate;
    uint8_t  reserved[8];
} ADCHeader;


FILE *openBinaryFile(const char *filename);

int readHeader(FILE *fp, ADCHeader *header);

int loadRecords(FILE *fp, ADCSample *samples, unsigned int record_count);


int writeResults(const char *filename,
                  const ChannelStats *stats, unsigned int channel_count,
                  const SequenceGap *gaps, unsigned int gap_count,
                  unsigned int total_records);

#endif /* IO_H */