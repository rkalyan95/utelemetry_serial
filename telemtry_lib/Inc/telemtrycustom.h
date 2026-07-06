#ifndef TELEMTRY_H
#define TELEMTRY_H

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif
#include <stdint.h>
#include <stdbool.h>
typedef uint8_t (*telemtry_custom_send_cb)(uint8_t *data, uint16_t len);
typedef uint8_t (*telemtry_custom_receive_cb)(uint8_t *data, uint16_t len);
extern telemtry_custom_send_cb telemtry_cb;
extern telemtry_custom_receive_cb telemtry_rx_cb;
typedef struct 
{
    
    uint16_t crc;
    uint8_t data_synch; //send data on 0xaa from mcu->pc
    uint8_t information_type; //float , char , int ,unsigned char , string , double ,array of bytes
    uint8_t information_id;
    uint8_t information_len;
    void *information_buffer;
    
}tel_information_t;

typedef struct
{
   uint8_t cmd_synch; //sent data on 0x55 from mcu->pc and reuqst for registration to python
   uint8_t cmd_id;
   uint8_t *tx_buffer;
   uint16_t crc;
}tel_cmd_t;

typedef enum
{
    TELEMTRY_TYPE_FLOAT = 0x01,
    TELEMTRY_TYPE_BYTES = 0x02,
    TELEMTRY_TYPE_INT32   = 0x03,
    TELEMTRY_TYPE_INT8    = 0x04,
    TELEMTRY_TYPE_STRING = 0x05,
    TELEMTRY_TYPE_INT16    = 0x06,
    TELEMTRY_TYPE_STRUCT  = 0x10,
    /* Invalid beyond this */
    TELEMTRY_TYPE_INVALID = 0xFF
}telemtry_type_t;

typedef enum
{
    TELEMTRY_ID_SYNCH = 0xAA,
    TELEMTRY_ID_CMD = 0x55,
}telemtry_id_t;


extern tel_information_t *telemtry_information;
extern tel_cmd_t *telemtry_cmd;
bool telemtry_init_callback(telemtry_custom_send_cb cb,telemtry_custom_receive_cb rx_cb);
void telemtry_custom_send(tel_information_t *serial_telemetry);
void telemtry_sendsync(void);
void telemtry_sendlen(void);
void telemtry_sendid(void);
void register_devices(tel_cmd_t **cmd , uint8_t number_of_devices);
void register_response(tel_cmd_t **cmd, uint8_t number_of_devices);
bool telemtry_wait_for_boot_sync(void);
uint8_t telemtry_send_boot_message(void);
void telemtry_senderror_info(char *error_msg, uint16_t len);
#endif /* TELEMTRY_H */