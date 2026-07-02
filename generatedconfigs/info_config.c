#include "../Inc/info_config.h"
#include "telemtrycustom.h"
#include <stdint.h>

static float temperature_data = 0.0;

static uint8_t status_flags_data = 0;

imu_reading_t imu_reading_data = {0};

battery_status_t battery_status_data = {0};

tel_information_t sensor_temperature = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_FLOAT,
    .information_id = TELEMTRY_ID_TEMPERATURE,
    .information_len = sizeof(temperature_data),
    .information_buffer = &temperature_data
};

tel_information_t sensor_status_flags = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_INT8,
    .information_id = TELEMTRY_ID_STATUS_FLAGS,
    .information_len = sizeof(status_flags_data),
    .information_buffer = &status_flags_data
};

tel_information_t sensor_imu_reading = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_IMU_READING,
    .information_len = sizeof(imu_reading_data),
    .information_buffer = &imu_reading_data
};

tel_information_t sensor_battery_status = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_BATTERY_STATUS,
    .information_len = sizeof(battery_status_data),
    .information_buffer = &battery_status_data
};

tel_cmd_t cmd_temperature = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_TEMPERATURE,
    .tx_buffer = (uint8_t *)"temperature",
    .crc = 0xFFFF
};

tel_cmd_t cmd_status_flags = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_STATUS_FLAGS,
    .tx_buffer = (uint8_t *)"status_flags",
    .crc = 0xFFFF
};

tel_cmd_t cmd_imu_reading = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_IMU_READING,
    .tx_buffer = (uint8_t *)"imu_reading:@ffffff",
    .crc = 0xFFFF
};

tel_cmd_t cmd_battery_status = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_BATTERY_STATUS,
    .tx_buffer = (uint8_t *)"battery_status:@Hhf",
    .crc = 0xFFFF
};

tel_cmd_t *sensor_array[TOTAL_TELEMTRY_ID-1] = {

    &cmd_temperature,
    &cmd_status_flags,
    &cmd_imu_reading,
    &cmd_battery_status,
};

tel_information_t *buffers_array[TOTAL_TELEMTRY_ID-1] = {

    &sensor_temperature,
    &sensor_status_flags,
    &sensor_imu_reading,
    &sensor_battery_status,
};
