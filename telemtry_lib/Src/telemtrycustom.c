#include "telemtrycustom.h"
#include "usart.h"
#include <string.h> 
#include "telemtry_transport.h"
/**
 * @file telemtrycustom.c
 * @brief Telemetry packet assembly and device registration helpers.
 *
 * This module builds and sends telemetry packets from the MCU to the Python UI,
 * performs CRC calculation, sends registration frames, and validates
 * registration acknowledgements from the host.
 */

tel_information_t *telemtry_information;
tel_cmd_t *telemtry_cmd;
telemtry_custom_send_cb telemtry_cb;
telemtry_custom_receive_cb telemtry_rx_cb;
/**
 * @brief Update a CRC-16 value over a byte buffer.
 *
 * @param crc_seed Initial CRC seed value.
 * @param data Pointer to the buffer to checksum.
 * @param length Number of bytes to process.
 * @return Updated CRC-16 value after processing the buffer.
 */
uint16_t telemtry_update_crc16(uint16_t crc_seed, const uint8_t *data, uint16_t length)
{
    uint16_t crc = crc_seed;
    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1) crc = (crc >> 1) ^ 0xA001;
            else         crc >>= 1;
        }
    }
    return crc;
}

/**
 * @brief Initialize the telemetry layer callbacks.
 *
 * @param info Pointer to a telemetry information descriptor.
 * @param cb Transmit callback used to send bytes over UART.
 * @param rx_cb Receive callback used to read bytes from UART.
 */
void telemtry_init_callback(telemtry_custom_send_cb cb,telemtry_custom_receive_cb rx_cb)
{
    if( cb == NULL || rx_cb == NULL)
    {
        return;
    }
    
    telemtry_cb = cb;
    telemtry_rx_cb = rx_cb; // Initialize receive callback to NULL
    
}

/**
 * Send the boot message using the registered transmit callback.
 * Returns number of bytes passed to the callback, or 0 on error (no callback).
 */
uint8_t telemtry_send_boot_message(void)
{
    static const char boot_msg[] = "Booting up...\r\n";
    if (telemtry_cb == NULL) return 0;
    return telemtry_cb((uint8_t *)boot_msg, (uint16_t)(sizeof(boot_msg) - 1));
}

/* Blocking wait: polls UART until sync byte 0xAA is received. */
void telemtry_wait_for_boot_sync(void)
{
    uint8_t boot_sync_signal = 0x00;
    while (boot_sync_signal != 0xAA)
    {
        telemtry_rx_cb(&boot_sync_signal, 1);
    }
}
/**
 * @brief Send a telemetry packet over the registered transport.
 *
 * The packet format is:
 *   [0xAA] [type] [id] [len] [payload...] [crc_lo] [crc_hi]
 *
 * @param serial_telemetry Pointer to the telemetry descriptor.
 */
void telemtry_custom_send(tel_information_t *serial_telemetry)
{
    if (serial_telemetry == NULL || telemtry_cb == NULL)
    {
        return;
    }
    telemtry_information = serial_telemetry;
    // 1. Compute rolling CRC
    uint16_t crc = 0xFFFF; // Start seed
    crc = telemtry_update_crc16(crc, &telemtry_information->information_id, 1);
    crc = telemtry_update_crc16(crc, &telemtry_information->information_len, 1);
    crc = telemtry_update_crc16(crc, &telemtry_information->information_type, 1);
    if (telemtry_information->information_buffer != NULL && telemtry_information->information_len > 0)
    {
        crc = telemtry_update_crc16(crc, (const uint8_t *)telemtry_information->information_buffer, telemtry_information->information_len);
    }
    
    // Save the computed CRC back into the struct
    telemtry_information->crc = crc;

    // 2. Stream out the packet sequentially over UART via your callback
    // Sync Byte
    telemtry_cb(&telemtry_information->data_synch, 1);
    telemtry_cb(&telemtry_information->information_type, 1);
    // ID
    telemtry_cb(&telemtry_information->information_id, 1);
    
    // Length
    telemtry_cb(&telemtry_information->information_len, 1);
    
    // Payload Data
    if (telemtry_information->information_buffer != NULL && telemtry_information->information_len > 0)
    {
        telemtry_cb((uint8_t *)telemtry_information->information_buffer, telemtry_information->information_len);
    }
    
    // CRC (2 Bytes)
    telemtry_cb((uint8_t *)&telemtry_information->crc, 2);
}

/**
 * @brief Send only the telemetry sync byte.
 *
 * This can be used to indicate a packet boundary or to keep the link alive.
 */
void telemtry_sendsync(void)
{
    if(telemtry_information == NULL || telemtry_cb == NULL)
    {
        return;
    }
    telemtry_cb(&telemtry_information->data_synch, 1);
}

/**
 * @brief Send only the telemetry packet length byte.
 */
void telemtry_sendlen(void)
{
    if(telemtry_information == NULL || telemtry_cb == NULL)
    {
        return;
    }
    telemtry_cb(&telemtry_information->information_len, 1);
}

/**
 * @brief Send only the telemetry packet ID byte.
 */
void telemtry_sendid(void)
{
    if(telemtry_information == NULL || telemtry_cb == NULL)
    {
        return;
    }
    telemtry_cb(&telemtry_information->information_id, 1);
}




/**
 * @brief Send only the telemetry packet CRC.
 */
void telemtry_sendcrc(void)
{
    if(telemtry_information == NULL || telemtry_cb == NULL)
    {
        return;
    }
    telemtry_cb((uint8_t *)&telemtry_information->crc, 2);
}

/**
 * @brief Register devices by sending their handshake commands.
 *
 * Each registration frame includes the command sync byte, command id,
 * command text, and CRC followed by CR/LF.
 *
 * @param cmd Array of device command descriptors.
 * @param number_of_devices Number of descriptors in the array.
 */
void register_devices(tel_cmd_t **cmd , uint8_t number_of_devices)
{
    if(*cmd == NULL)
    {
        return;
    }
    uint16_t crc;
    uint8_t cr = 0x0D;
    uint8_t lf = 0x0A;
    for(int i = 0; i < number_of_devices; i++)
    {
         crc = 0xFFFF; // Start seed
        //cmd[i]->cmd_id = i;
        crc = telemtry_update_crc16(crc, &cmd[i]->cmd_synch, 1);
        crc = telemtry_update_crc16(crc, &cmd[i]->cmd_id, 1);
        if(cmd[i]->tx_buffer != NULL)
        {
            crc = telemtry_update_crc16(crc, cmd[i]->tx_buffer, strlen((const char *)cmd[i]->tx_buffer));
        }
        cmd[i]->crc = crc;
        telemtry_cb(&cmd[i]->cmd_synch, 1);
        telemtry_cb(&cmd[i]->cmd_id, 1);
        if(cmd[i]->tx_buffer != NULL)
        {
            telemtry_cb(cmd[i]->tx_buffer, strlen((const char *)cmd[i]->tx_buffer));
        }
        telemtry_cb((uint8_t *)&cmd[i]->crc, 2);
        //send delimiter for each device registration
        telemtry_cb(&cr, 1);
        telemtry_cb(&lf, 1);

    }
    
}

//it will check the received crc from python ui and compare 
//to validate the device registration
/**
 * @brief Receive and validate registration CRC acknowledgements.
 *
 * Reads two bytes from the receive callback for each device and compares
 * them against the previously computed registration CRC.
 *
 * @param cmd Array of device command descriptors.
 * @param number_of_devices Number of descriptors in the array.
 */
void register_response(tel_cmd_t **cmd, uint8_t number_of_devices)
{

    if(*cmd == NULL || telemtry_rx_cb == NULL)
    {
        return;
    }
    uint8_t crc[2] = {0xFF,0xFF}; 
    uint8_t ack_success[2] = {0xAA, 0x55};
    uint8_t ack_failed[2] = {0xDE, 0xAD};
    for(int i = 0; i < number_of_devices; i++)
    {
        telemtry_rx_cb((uint8_t *)&crc, 2);

        if(crc[0] == (cmd[i]->crc & 0xFF) && crc[1] == ((cmd[i]->crc >> 8) & 0xFF))
        {
            // Device registration successful
            telemtry_cb(ack_success, 2);
        }
        else
        {
            // Device registration failed
            telemtry_cb(ack_failed, 2);
        }
        //reset for next
        crc[0] = 0xFF;
        crc[1] = 0xFF;
    }

}