/*
 * telemtry_stm32.c
 *
 * Implementation file that includes the public telemetry header.
 */

#include "../Inc/telemtry_stm32.h"
#include "usart.h"
#include <string.h>
#include "telemtrycustom.h"
#include "info_config.h"
static uint8_t mysensor_send(uint8_t *data, uint16_t len)
{
    return HAL_UART_Transmit(&huart1, data, len, 1000);
}

static uint8_t mysensor_receive(uint8_t *data, uint16_t len)
{
    return HAL_UART_Receive(&huart1, data, len, 1000);
}


void telemtry_init(uint8_t controller_id)
{
	/* Initialize telemetry peripherals here (UART, DMA, etc.) */
    switch(controller_id)
    {
        case 1://stm32
            telemtry_custom_init(mysensor_send, mysensor_receive);
            break;
        default:
            // Handle unsupported controller IDs
            break;
    }
    
}

void telemtry_configure(uint8_t controller_id)
{
    switch(controller_id)
    {
        case 1://stm32
                register_devices(sensor_array, TOTAL_TELEMTRY_ID-1);
                register_response(sensor_array, TOTAL_TELEMTRY_ID-1);
                break;
        default:
            // Handle unsupported controller IDs
                break;
    }

}

void telemtry_send(uint8_t controller_id, tel_information_t **buffers, uint8_t buffer_id)
{
    switch(controller_id)
    {
        case 1://stm32
            telemtry_custom_send((buffers[buffer_id]));
            break;
        default:
            // Handle unsupported controller IDs
            break;
    }
}