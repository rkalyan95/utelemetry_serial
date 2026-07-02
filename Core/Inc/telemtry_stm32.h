/*
 * telemtry_stm32.h
 *
 * Protective header: include guard + C++ compatibility
 */

#ifndef TELEMTRY_STM32_H
#define TELEMTRY_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "telemtrycustom.h"
/* Public telemetry API declarations go here */

/** Initialize telemetry subsystem on STM32. */
void telemtry_init(uint8_t controller_id);
void telemtry_configure(uint8_t controller_id);
void telemtry_send(uint8_t controller_id, tel_information_t **buffers, uint8_t buffer_id);

#ifdef __cplusplus
}
#endif

#endif /* TELEMTRY_STM32_H */

