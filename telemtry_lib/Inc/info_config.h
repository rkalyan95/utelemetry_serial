/* Auto-generated telemetry config header. */
#ifndef INFO_CONFIG_H
#define INFO_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "telemtrycustom.h"

/* Generated telemetry IDs for sensors and commands. */
enum
{
    TELEMTRY_ID_USONIC = 0x01,
    /* Add only above this line */
    TOTAL_TELEMTRY_ID,
};

typedef struct {
    uint32_t sonic_timestamp;
    uint32_t sonic_duration;
    uint16_t distance_mm;
    char sonic_name[6];
} usonic_t;


extern tel_cmd_t *sensor_array[TOTAL_TELEMTRY_ID-1];
extern tel_information_t *buffers_array[TOTAL_TELEMTRY_ID-1];

#endif /* INFO_CONFIG_H */
