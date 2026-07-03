#include "../Inc/info_config.h"
#include "telemtrycustom.h"
#include <stdint.h>

static float test_data = 0;

tel_information_t sensor_test = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_FLOAT,
    .information_id = TELEMTRY_ID_TEST,
    .information_len = sizeof(test_data),
    .information_buffer = &test_data
};

tel_cmd_t cmd_test = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_TEST,
    .tx_buffer = (uint8_t *)"test",
    .crc = 0xFFFF
};

tel_cmd_t *sensor_array[TOTAL_TELEMTRY_ID-1] = {

    &cmd_test,
};

tel_information_t *buffers_array[TOTAL_TELEMTRY_ID-1] = {

    &sensor_test,
};
