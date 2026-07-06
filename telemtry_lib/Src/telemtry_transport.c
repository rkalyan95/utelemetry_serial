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

bool sync_status = false;
bool hw_init = false;
bool callback_init = false;

void telemtry_init(void)
{
    if(1==telemtry_hw_init())
    {
        hw_init = true;
    }
    else
    {
        hw_init = false;
        return;
    }
    if(1==telemtry_init_callback(telemtry_hw_send, telemtry_hw_receive))
    {
        callback_init = true;
    }
    else
    {
        callback_init = false;
        return;
    }

    telemtry_send_boot_message();

    if(1==telemtry_wait_for_boot_sync())
    {
        sync_status = true;
    }
    else
    {
        sync_status = false;
    }
    telemtry_configure();
}

bool get_sync_status(void)
{
    return sync_status;
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

uint8_t telemtry_send( tel_information_t **buffers, uint8_t buffer_id)
{
    if(buffers == NULL || *buffers == NULL || buffer_id >= (TOTAL_TELEMTRY_ID - 1))
    {
        return 0; // Invalid input
    }
    else if(sync_status == false)
    {
        telemtry_senderror_info("Sync not established. Please check the connection and restart the device.\r\n", 80);
        return 2; // Sync not established
    }
    else if(hw_init == false)
    {
        telemtry_senderror_info("Hardware not initialized. Please check the hardware and restart the device.\r\n", 80);
        return 3; // Hardware not initialized
    }
    else if(callback_init == false)
    {
        telemtry_senderror_info("Callback not initialized. Please check the callback and restart the device.\r\n", 80);
        return 4; // Callback not initialized
    }
    else
    {
        telemtry_custom_send((buffers[buffer_id]));
        return 1; // Assuming 1 byte sent for simplicity; adjust as needed
    }
    
}