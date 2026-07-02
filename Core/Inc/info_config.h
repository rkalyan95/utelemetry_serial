/*
 * info_config.h
 *
 * Protective header: include guard + C++ compatibility
 */

#ifndef INFO_CONFIG_H
#define INFO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "telemtrycustom.h"
/* Configuration and information ID definitions go here. */

enum 
{
    TELEMTRY_ID_IMU = 0x01,
    TELEMTRY_ID_BMP = 0x02,
    TELEMTRY_ID_GPS = 0x03,
    TELEMTRY_ID_SYS_STATUS = 0x04,
    TELEMTRY_ID_MOTOR = 0x05,
    TELEMTRY_ID_MIXER = 0x06,
    TELEMTRY_ID_RANDOM = 0x07,
    TELEMTRY_ID_STRING = 0x08,
    TELEMTRY_ID_BYTES = 0x09,
    /* Add only above this line */
    TOTAL_TELEMTRY_ID,
};

typedef struct  {
    float x;
    float y;
    float z;
}imu_data_t;

typedef struct  {
    uint16_t voltage_mv;
    int16_t  current_ma;
    int8_t   temperature_c;
}battery_status_t;

typedef struct {
    double latitude;  // 8 bytes
    double longitude; // 8 bytes
    uint8_t satellites; // 1 byte
} packed_gps_t;

typedef struct  {
    uint32_t uptime_seconds; // 4 bytes
    uint16_t error_flags;    // 2 bytes
    uint8_t  mcu_load_pct;   // 1 byte
} packed_sys_status_t;

typedef struct {
    int8_t   flight_mode; // 1 byte (+ 3 internal padding bytes added here)
    uint32_t checksum;    // 4 bytes
    uint8_t  state_mask;  // 1 byte (+ 3 internal padding bytes added at the end)
} unpacked_mixer_t;

typedef struct {
    int8_t  direction;    // 1 byte (+ 1 internal padding byte added here)
    int16_t raw_ticks;    // 2 bytes
    float   phase_current;// 4 bytes
} unpacked_motor_t;

extern tel_cmd_t *sensor_array[9];
extern tel_information_t *buffers_array[9];
#ifdef __cplusplus
}
#endif

#endif /* INFO_CONFIG_H */

