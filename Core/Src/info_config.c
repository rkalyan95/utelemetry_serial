/*
 * info_config.c
 *
 * Implementation for info/config subsystem.
 */

#include "../Inc/info_config.h"
#include <string.h>
#include <stdio.h>
#include "telemtrycustom.h"

imu_data_t mpu6050 = { .x = 1.02f, .y = -0.54f, .z = 9.81f };
battery_status_t bmp280 = { .voltage_mv = 3700, .current_ma = 500, .temperature_c = 25 };

packed_gps_t gps_test = {
    .latitude = 28.6139,
    .longitude = 77.2090,
    .satellites = 12
};

// Handshake: "SysStatus:IHB" -> Expected output: (3600, 1024, 45)
packed_sys_status_t sys_test = {
    .uptime_seconds = 3600,
    .error_flags = 1024,
    .mcu_load_pct = 45
};

// Handshake: "Motor:bhf" -> Expected output: (-1, 4500, 14.250)
unpacked_motor_t motor_test = {
    .direction = -1,
    .raw_ticks = 4500,
    .phase_current = 14.25f
};

// Handshake: "Mixer:bIB" -> Expected output: (2, 54321, 1)
unpacked_mixer_t mixer_test = {
    .flight_mode = 2,
    .checksum = 54321,
    .state_mask = 1
};


tel_information_t mysensor1 = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_IMU, // IMU
    .information_len = sizeof(mpu6050),
    .information_buffer = &mpu6050
};

tel_information_t mysensor2 = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_BMP, // BMP
    .information_len = sizeof(bmp280),
    .information_buffer = &bmp280
};

tel_information_t mysensor3 = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_GPS, // GPS 👈 Fix ID match
    .information_len = sizeof(gps_test),
    .information_buffer = &gps_test
};

tel_information_t mysensor4 = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_SYS_STATUS, // SysStatus 👈 Fix ID match
    .information_len = sizeof(sys_test),
    .information_buffer = &sys_test
};

tel_information_t mysensor5 = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_MOTOR, // Motor 👈 Fix ID match
    .information_len = sizeof(motor_test),
    .information_buffer = &motor_test
};

tel_information_t mysensor6 = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_MIXER, // Mixer 👈 Fix ID match
    .information_len = sizeof(mixer_test),
    .information_buffer = &mixer_test
};

float sensor_7 = 3.14159;

tel_information_t mysensor7 = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_FLOAT,
    .information_id = TELEMTRY_ID_RANDOM, // Mixer 👈 Fix ID match
    .information_len = sizeof(sensor_7),
    .information_buffer = &sensor_7
};

static const char sensor_8[] = "Hello, World!";
tel_information_t mysensor8 = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRING,
    .information_id = TELEMTRY_ID_STRING, // Mixer 👈 Fix ID match
    .information_len = sizeof(sensor_8) - 1,
    .information_buffer = &sensor_8
};

uint8_t sensor_9[5] = {1, 2, 3, 4, 5};
tel_information_t mysensor9 = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_BYTES,
    .information_id = TELEMTRY_ID_BYTES, // Mixer 👈 Fix ID match
    .information_len = sizeof(sensor_9),
    .information_buffer = &sensor_9
};



tel_cmd_t imu_cmd = {
    
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_IMU,
    .tx_buffer = (uint8_t *)"IMU:@fff",
    .crc = 0xFFFF
};

tel_cmd_t bmp_cmd = {
    
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_BMP,
    .tx_buffer = (uint8_t *)"BMP:@Hhb",
    .crc = 0xFFFF
};
tel_cmd_t gps_cmd = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_GPS,
    .tx_buffer = (uint8_t *)"GPS:@ddb",
    .crc = 0xFFFF
};
tel_cmd_t sys_status_cmd = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_SYS_STATUS,
    .tx_buffer = (uint8_t *)"SysStatus:@IHB",
    .crc = 0xFFFF
};
tel_cmd_t motor_cmd = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_MOTOR,
    .tx_buffer = (uint8_t *)"Motor:@bhf",
    .crc = 0xFFFF
};
tel_cmd_t mixer_cmd = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_MIXER,
    .tx_buffer = (uint8_t *)"Mixer:@bIB",
    .crc = 0xFFFF
};

tel_cmd_t random_cmd = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_RANDOM,
    .tx_buffer = (uint8_t *)"Random:@f",
    .crc = 0xFFFF
};

tel_cmd_t string_cmd = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_STRING,
    .tx_buffer = (uint8_t *)"String:@s",
    .crc = 0xFFFF
};

tel_cmd_t bytes_cmd = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_BYTES,
    .tx_buffer = (uint8_t *)"Bytes:@",
    .crc = 0xFFFF
};

tel_cmd_t *sensor_array[9] = { &imu_cmd, &bmp_cmd , &gps_cmd, &sys_status_cmd, &motor_cmd, &mixer_cmd ,&random_cmd, &string_cmd,&bytes_cmd};
tel_information_t *buffers_array[9] = { &mysensor1, &mysensor2 , &mysensor3, &mysensor4, &mysensor5, &mysensor6 ,&mysensor7, &mysensor8,&mysensor9};
