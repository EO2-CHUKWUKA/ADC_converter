#include <stdlib.h>
#include "io.h"

FILE *openBinaryFile(const char *filename)
{
    FILE *fp = fopen(filename, "rb"); /* "rb": binary mode, so no
                                          newline translation corrupts
                                          the raw bytes */
    if (fp == NULL)
    {
        printf("File opening failed: '%s'\n", filename);
    }
    return fp;
}

int readHeader(FILE *fp, ADCHeader *header)
{
    size_t itemsRead = fread(header, sizeof(ADCHeader), 1, fp);

    if (itemsRead != 1)
    {
        printf("Header read failed: could not read %zu bytes "
               "(file too short / truncated)\n", sizeof(ADCHeader));
        return 0;
    }

    if (header->magic != EXPECTED_MAGIC)
    {
        printf("Header read failed: bad magic number 0x%X (expected 0x%X). "
               "This is not a valid ADC log file.\n",
               header->magic, EXPECTED_MAGIC);
        return 0;
    }

    if (header->version != EXPECTED_VERSION)
    {
        printf("Header read failed: unsupported version %u (expected %u)\n",
               header->version, EXPECTED_VERSION);
        return 0;
    }

    if (header->channel_count != EXPECTED_CHANNEL_COUNT)
    {
        printf("Header read failed: unexpected channel count %u (expected %u)\n",
               header->channel_count, EXPECTED_CHANNEL_COUNT);
        return 0;
    }

    if (header->record_count == 0)
    {
        printf("Header read failed: record_count is 0, nothing to process\n");
        return 0;
    }

    return 1;
}

/* ------------------------------------------------------------------
 * RawRecord: EXACT 16-byte on-disk layout of one record, kept private
 * to io.c (not in io.h) because nothing outside this file needs to
 * know about it. It exists purely so fread() has something whose
 * memory layout matches the file exactly, field for field, in the
 * same order the file has them.
 *
 * ADCSample (adc.h) is NOT used directly for fread() because it has
 * an extra 'voltage' field the file doesn't contain -- if fread()
 * wrote straight into an ADCSample-shaped buffer, 'voltage' sitting
 * in the middle of the struct would shift every field after it out
 * of alignment with the file, and they'd all come out as garbage.
 * So: fread() into RawRecord here, then copy field-by-field into the
 * ADCSample the rest of the program uses.
 * ------------------------------------------------------------------ */
typedef struct __attribute__((packed))
{
    float    timestamp;
    uint8_t  channel_id;
    uint16_t raw_value;
    int16_t  temperature;
    uint8_t  status_flags;
    uint32_t sequence_number;
    uint8_t  reserved[2];
} RawRecord;

int loadRecords(FILE *fp, ADCSample *samples, unsigned int record_count)
{
    RawRecord raw; /* one 16-byte on-disk record at a time */

    for (unsigned int i = 0; i < record_count; i++)
    {
        size_t itemsRead = fread(&raw, sizeof(RawRecord), 1, fp);

        if (itemsRead != 1)
        {
            printf("Record loading failed: expected %u records, "
                   "file ended after %u\n", record_count, i);
            return 0;
        }

        ADCSample *dst = samples + i; /* pointer arithmetic instead of
                                          samples[i] */
        dst->timestamp        = raw.timestamp;
        dst->channel_id       = raw.channel_id;
        dst->raw_value        = raw.raw_value;
        dst->voltage          = 0.0f; /* filled in later by
                                          convertToVoltage() in adc.c --
                                          io.c only does I/O */
        dst->temperature      = raw.temperature;
        dst->status_flags     = raw.status_flags;
        dst->sequence_number  = raw.sequence_number;
    }

    return 1;
}

int writeResults(const char *filename,
                  const ChannelStats *stats, unsigned int channel_count,
                  const SequenceGap *gaps, unsigned int gap_count,
                  unsigned int total_records)
{
    FILE *fp = fopen(filename, "w"); /* text mode: results.txt is a
                                         human-readable report */
    if (fp == NULL)
    {
        printf("Could not open '%s' for writing\n", filename);
        return 0;
    }

    fprintf(fp, "ADC Sensor Data Processor - Results\n");
    fprintf(fp, "====================================\n");
    fprintf(fp, "Total records processed: %u\n\n", total_records);

    for (unsigned int c = 0; c < channel_count; c++)
    {
        const ChannelStats *s = &stats[c];
        fprintf(fp, "Channel %u\n", s->channel_id);
        fprintf(fp, "----------\n");
        fprintf(fp, "  Sample count       : %u\n", s->count);
        fprintf(fp, "  Mean voltage       : %.4f V\n", s->mean_voltage);
        fprintf(fp, "  Min voltage        : %.4f V\n", s->min_voltage);
        fprintf(fp, "  Max voltage        : %.4f V\n", s->max_voltage);
        fprintf(fp, "  Std deviation      : %.4f V\n", s->stddev_voltage);
        fprintf(fp, "  Overvoltage events (>3.0V)  : %u\n", s->overvoltage_count);
        fprintf(fp, "  Undervoltage events (<0.3V) : %u\n", s->undervoltage_count);
        fprintf(fp, "  Sensor fault flags (bit 0)  : %u\n", s->sensor_fault_count);
        fprintf(fp, "  Out-of-range flags (bit 1)  : %u\n", s->out_of_range_count);
        fprintf(fp, "\n");
    }

    fprintf(fp, "Sequence Integrity\n");
    fprintf(fp, "-------------------\n");
    if (gap_count == 0)
    {
        fprintf(fp, "  No gaps detected. All sequence numbers increment by 1.\n");
    }
    else
    {
        fprintf(fp, "  %u gap(s) detected:\n", gap_count);
        for (unsigned int i = 0; i < gap_count; i++)
        {
            fprintf(fp,
                    "    At record index %u: expected sequence %u, found %u "
                    "(%u record(s) missing)\n",
                    gaps[i].record_index,
                    gaps[i].expected_sequence,
                    gaps[i].found_sequence,
                    gaps[i].found_sequence - gaps[i].expected_sequence);
        }
    }

    fclose(fp);
    return 1;
}