#include "telemtry_hw.h"
#include "driver/uart.h"

// Define port and pins according to your ESP32 hardware
#define UART_NUM_ESP UART_NUM_1

bool telemtry_hw_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    // Configure UART parameters
    if (uart_param_config(UART_NUM_ESP, &uart_config) != ESP_OK) {
        return false;
    }
    uart_set_pin(UART_NUM_1, 18, 19, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // Install the driver with a 256-byte RX buffer
    if (uart_driver_install(UART_NUM_1, 2048, 0, 0, NULL, 0) != ESP_OK) {
        return false;
    }
    
    
    return true;
}

uint8_t telemtry_hw_send(uint8_t *data, uint16_t len) {
    if (data == NULL || len == 0) return 0;
    
    // Send bytes; returns the number of bytes written to the UART buffer
    telemtry_hw_flush();
    int bytes_written = uart_write_bytes(UART_NUM_ESP, (const char*)data, len);
    return (bytes_written > 0) ? (uint8_t)bytes_written : 0;
}

uint8_t telemtry_hw_receive(uint8_t *data, uint16_t len) {
    if (data == NULL || len == 0) return 0;
    
    // Read bytes from buffer with a 1000ms timeout
    int length = uart_read_bytes(UART_NUM_ESP, data, len, 1000 / portTICK_PERIOD_MS);
    
    return (length > 0) ? (uint8_t)length : 0;
}

void telemtry_hw_flush(void) {
    // Wait for the TX buffer to be empty before returning
    uart_wait_tx_done(UART_NUM_ESP, 1000 / portTICK_PERIOD_MS);
}