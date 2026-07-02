#include "../Inc/info_config.h"
#include "telemtrycustom.h"
#include <stdint.h>

static float temperature_data = 0.0;

static uint8_t status_flags_data = 0;

imu_reading_t imu_reading_data = {0};

battery_status_t battery_status_data = {0};

static char device_label_data[9] = "STM32-01";

static uint8_t raw_data_data[8] = {0};

motor_t motor_data = {0};

test_string_t test_string_data = {0};

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

tel_information_t sensor_device_label = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRING,
    .information_id = TELEMTRY_ID_DEVICE_LABEL,
    .information_len = sizeof(device_label_data) - 1,
    .information_buffer = device_label_data
};

tel_information_t sensor_raw_data = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_BYTES,
    .information_id = TELEMTRY_ID_RAW_DATA,
    .information_len = sizeof(raw_data_data),
    .information_buffer = raw_data_data
};

tel_information_t sensor_motor = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_MOTOR,
    .information_len = sizeof(motor_data),
    .information_buffer = &motor_data
};

tel_information_t sensor_test_string = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_TEST_STRING,
    .information_len = sizeof(test_string_data),
    .information_buffer = &test_string_data
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

tel_cmd_t cmd_device_label = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_DEVICE_LABEL,
    .tx_buffer = (uint8_t *)"device_label",
    .crc = 0xFFFF
};

tel_cmd_t cmd_raw_data = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_RAW_DATA,
    .tx_buffer = (uint8_t *)"raw_data",
    .crc = 0xFFFF
};

tel_cmd_t cmd_motor = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_MOTOR,
    .tx_buffer = (uint8_t *)"motor:@Bfi1s",
    .crc = 0xFFFF
};

tel_cmd_t cmd_test_string = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_TEST_STRING,
    .tx_buffer = (uint8_t *)"test_string:@fHh1s1S",
    .crc = 0xFFFF
};

tel_cmd_t *sensor_array[TOTAL_TELEMTRY_ID-1] = {

    &cmd_temperature,
    &cmd_status_flags,
    &cmd_imu_reading,
    &cmd_battery_status,
    &cmd_device_label,
    &cmd_raw_data,
    &cmd_motor,
    &cmd_test_string,
};

tel_information_t *buffers_array[TOTAL_TELEMTRY_ID-1] = {

    &sensor_temperature,
    &sensor_status_flags,
    &sensor_imu_reading,
    &sensor_battery_status,
    &sensor_device_label,
    &sensor_raw_data,
    &sensor_motor,
    &sensor_test_string,
};
