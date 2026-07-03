#ifndef TELEMTRY_HW_H
#define TELEMTRY_HW_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Initialize the transport hardware for telemetry.
 *
 * This should configure the underlying UART, USB, CAN, or other link
 * and return true when the transport is ready.
 */
bool telemtry_hw_init(void);

/**
 * @brief Send a buffer over the telemetry transport.
 *
 * @param data Pointer to bytes to send.
 * @param len Number of bytes to send.
 * @return number of bytes transmitted, or 0 on error.
 */
uint8_t telemtry_hw_send(const uint8_t *data, uint16_t len);

/**
 * @brief Receive bytes from the telemetry transport.
 *
 * @param data Destination buffer.
 * @param len Maximum number of bytes to receive.
 * @return number of bytes received, or 0 on timeout/error.
 */
uint8_t telemtry_hw_receive(uint8_t *data, uint16_t len);

/**
 * @brief Flush the telemetry transport buffers if supported.
 */
void telemtry_hw_flush(void);

#ifdef __cplusplus
}
#endif

#endif /* TELEMTRY_HW_H */
