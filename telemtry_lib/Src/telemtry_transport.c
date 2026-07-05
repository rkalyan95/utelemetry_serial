/*
 * telemtry_stm32.c
 *
 * Implementation file that includes the public telemetry header.
 */

#include "../Inc/telemtry_transport.h"

#include <string.h>
#include "telemtrycustom.h"
#include "info_config.h"
#include "telemtry_hw.h"

void telemtry_init(void)
{
    telemtry_hw_init();
    telemtry_init_callback(telemtry_hw_send, telemtry_hw_receive);
}

void telemtry_configure(void)
{
    if(TOTAL_TELEMTRY_ID<=1)
    {
        return;
    }
    register_devices(sensor_array, TOTAL_TELEMTRY_ID-1);
    register_response(sensor_array, TOTAL_TELEMTRY_ID-1);
}

void telemtry_send( tel_information_t **buffers, uint8_t buffer_id)
{
    if(buffers == NULL || *buffers == NULL || buffer_id >= (TOTAL_TELEMTRY_ID - 1))
    {
        return;
    }
    telemtry_custom_send((buffers[buffer_id]));
}