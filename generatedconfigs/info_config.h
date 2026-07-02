/* Auto-generated telemetry config header. */
#ifndef INFO_CONFIG_H
#define INFO_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "telemtrycustom.h"

/* Generated telemetry IDs for sensors and commands. */
enum
{
    TELEMTRY_ID_TEMPERATURE = 0x01,
    TELEMTRY_ID_STATUS_FLAGS = 0x02,
    TELEMTRY_ID_IMU_READING = 0x03,
    TELEMTRY_ID_BATTERY_STATUS = 0x04,
    /* Add only above this line */
    TOTAL_TELEMTRY_ID,
};

typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} imu_reading_t;

typedef struct {
    uint16_t voltage_mv;
    int16_t current_ma;
    float temperature_c;
} battery_status_t;


extern tel_cmd_t *sensor_array[TOTAL_TELEMTRY_ID-1];
extern tel_information_t *buffers_array[TOTAL_TELEMTRY_ID-1];

#endif /* INFO_CONFIG_H */
