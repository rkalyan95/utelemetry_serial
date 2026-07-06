#include "../Inc/info_config.h"
#include "telemtrycustom.h"
#include <stdint.h>

usonic_t usonic_data = { 0, 0, 0, "hr-04" };

tel_information_t sensor_usonic = {
    .data_synch = TELEMTRY_ID_SYNCH,
    .information_type = TELEMTRY_TYPE_STRUCT,
    .information_id = TELEMTRY_ID_USONIC,
    .information_len = sizeof(usonic_data),
    .information_buffer = &usonic_data
};

tel_cmd_t cmd_usonic = {
    .cmd_synch = TELEMTRY_ID_CMD,
    .cmd_id = TELEMTRY_ID_USONIC,
    .tx_buffer = (uint8_t *)"usonic:@IIH6S",
    .crc = 0xFFFF
};

tel_cmd_t *sensor_array[TOTAL_TELEMTRY_ID-1] = {

    &cmd_usonic,
};

tel_information_t *buffers_array[TOTAL_TELEMTRY_ID-1] = {

    &sensor_usonic,
};
