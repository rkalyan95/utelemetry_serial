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
void telemtry_init(void);
void telemtry_configure(void);

uint8_t telemtry_send( tel_information_t **buffers, uint8_t buffer_id);

extern bool sync_status;
extern bool hw_init;
extern bool callback_init;
#ifdef __cplusplus
}
#endif

#endif /* TELEMTRY_STM32_H */

