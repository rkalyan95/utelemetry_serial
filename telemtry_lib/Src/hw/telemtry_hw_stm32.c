#include "telemtry_hw.h"
#include "usart.h"
#include "stm32l4xx_hal.h"

#include <string.h>


bool telemtry_hw_init(void)
{
    MX_USART1_UART_Init();
    return true;
}

uint8_t telemtry_hw_send(uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0)
    {
        return 0;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t *)data, len, 1000);
    return (status == HAL_OK) ? len : 0;
}

uint8_t telemtry_hw_receive(uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0)
    {
        return 0;
    }

    HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, data, len, 1000);
    return (status == HAL_OK) ? len : 0;
}

void telemtry_hw_flush(void)
{
    __HAL_UART_FLUSH_DRREGISTER(&huart1);
}
