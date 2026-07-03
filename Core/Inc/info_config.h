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
    TELEMTRY_ID_DEVICE_LABEL = 0x05,
    TELEMTRY_ID_RAW_DATA = 0x06,
    TELEMTRY_ID_MOTOR = 0x07,
    TELEMTRY_ID_TEST_STRING = 0x08,
    TELEMTRY_ID_TEST_FINAL = 0x09,
    TELEMTRY_ID_TEST_STRINGS = 0x0A,
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

typedef struct {
    uint8_t motor_1;
    float motor_2;
    int32_t motor_3;
    uint8_t motor_4[1];
} motor_t;

typedef struct {
    float first;
    uint16_t second;
    int16_t third;
    uint8_t fourth[1];
    char fifth[1];
} test_string_t;

typedef struct {
    float fil_1;
    int16_t fil_2;
    uint8_t fil_3[4];
} test_final_t;

typedef struct {
    uint32_t value1;
    float value2;
    char value3[17];
    uint8_t value4[5];
} test_strings_t;


extern tel_cmd_t *sensor_array[TOTAL_TELEMTRY_ID-1];
extern tel_information_t *buffers_array[TOTAL_TELEMTRY_ID-1];

#endif /* INFO_CONFIG_H */
