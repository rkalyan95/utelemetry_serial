/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "telemtrycustom.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
float sensor_1 = 123.456;
float sensor_2 = 789.012;

typedef struct  {
    float x;
    float y;
    float z;
}imu_data_t;

typedef struct  {
    uint16_t voltage_mv;
    int16_t  current_ma;
    int8_t   temperature_c;
}battery_status_t;

typedef struct {
    double latitude;  // 8 bytes
    double longitude; // 8 bytes
    uint8_t satellites; // 1 byte
} packed_gps_t;

typedef struct  {
    uint32_t uptime_seconds; // 4 bytes
    uint16_t error_flags;    // 2 bytes
    uint8_t  mcu_load_pct;   // 1 byte
} packed_sys_status_t;

typedef struct {
    int8_t   flight_mode; // 1 byte (+ 3 internal padding bytes added here)
    uint32_t checksum;    // 4 bytes
    uint8_t  state_mask;  // 1 byte (+ 3 internal padding bytes added at the end)
} unpacked_mixer_t;

typedef struct {
    int8_t  direction;    // 1 byte (+ 1 internal padding byte added here)
    int16_t raw_ticks;    // 2 bytes
    float   phase_current;// 4 bytes
} unpacked_motor_t;

imu_data_t mpu6050 = { .x = 1.02f, .y = -0.54f, .z = 9.81f };
battery_status_t bmp280 = { .voltage_mv = 3700, .current_ma = 500, .temperature_c = 25 };
// Handshake: "GPS:ddb" -> Expected output: (28.614, 77.209, 12)
packed_gps_t gps_test = {
    .latitude = 28.6139,
    .longitude = 77.2090,
    .satellites = 12
};

// Handshake: "SysStatus:IHB" -> Expected output: (3600, 1024, 45)
packed_sys_status_t sys_test = {
    .uptime_seconds = 3600,
    .error_flags = 1024,
    .mcu_load_pct = 45
};

// Handshake: "Motor:bhf" -> Expected output: (-1, 4500, 14.250)
unpacked_motor_t motor_test = {
    .direction = -1,
    .raw_ticks = 4500,
    .phase_current = 14.25f
};

// Handshake: "Mixer:bIB" -> Expected output: (2, 54321, 1)
unpacked_mixer_t mixer_test = {
    .flight_mode = 2,
    .checksum = 54321,
    .state_mask = 1
};

tel_information_t mysensor1 = {
    .data_synch = 0xAA,
    .information_type = 0x10,
    .information_id = 0x01, // IMU
    .information_len = sizeof(mpu6050),
    .information_buffer = &mpu6050
};

tel_information_t mysensor2 = {
    .data_synch = 0xAA,
    .information_type = 0x10,
    .information_id = 0x02, // BMP
    .information_len = sizeof(bmp280),
    .information_buffer = &bmp280
};

tel_information_t mysensor3 = {
    .data_synch = 0xAA,
    .information_type = 0x10,
    .information_id = 0x03, // GPS 👈 Fix ID match
    .information_len = sizeof(gps_test),
    .information_buffer = &gps_test
};

tel_information_t mysensor4 = {
    .data_synch = 0xAA,
    .information_type = 0x10,
    .information_id = 0x04, // SysStatus 👈 Fix ID match
    .information_len = sizeof(sys_test),
    .information_buffer = &sys_test
};

tel_information_t mysensor5 = {
    .data_synch = 0xAA,
    .information_type = 0x10,
    .information_id = 0x05, // Motor 👈 Fix ID match
    .information_len = sizeof(motor_test),
    .information_buffer = &motor_test
};

tel_information_t mysensor6 = {
    .data_synch = 0xAA,
    .information_type = 0x10,
    .information_id = 0x06, // Mixer 👈 Fix ID match
    .information_len = sizeof(mixer_test),
    .information_buffer = &mixer_test
};

tel_cmd_t imu_cmd = {
    
    .cmd_synch = 0x55,
    .cmd_id = 0x01,
    .tx_buffer = (uint8_t *)"IMU:@fff",
    .crc = 0xFFFF
};

tel_cmd_t bmp_cmd = {
    
    .cmd_synch = 0x55,
    .cmd_id = 0x02,
    .tx_buffer = (uint8_t *)"BMP:@Hhb",
    .crc = 0xFFFF
};
tel_cmd_t gps_cmd = {
    .cmd_synch = 0x55,
    .cmd_id = 0x03,
    .tx_buffer = (uint8_t *)"GPS:@ddb",
    .crc = 0xFFFF
};
tel_cmd_t sys_status_cmd = {
    .cmd_synch = 0x55,
    .cmd_id = 0x04,
    .tx_buffer = (uint8_t *)"SysStatus:@IHB",
    .crc = 0xFFFF
};
tel_cmd_t motor_cmd = {
    .cmd_synch = 0x55,
    .cmd_id = 0x05,
    .tx_buffer = (uint8_t *)"Motor:@bhf",
    .crc = 0xFFFF
};
tel_cmd_t mixer_cmd = {
    .cmd_synch = 0x55,
    .cmd_id = 0x06,
    .tx_buffer = (uint8_t *)"Mixer:@bIB",
    .crc = 0xFFFF
};

tel_cmd_t *sensor_array[6] = { &imu_cmd, &bmp_cmd , &gps_cmd, &sys_status_cmd, &motor_cmd, &mixer_cmd };

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t mysensor_send(uint8_t *data, uint16_t len)
{
    return HAL_UART_Transmit(&huart1, data, len, 1000);
}

uint8_t mysensor_receive(uint8_t *data, uint16_t len)
{
    return HAL_UART_Receive(&huart1, data, len, 1000);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  telemtry_custom_init(&mysensor1, mysensor_send,mysensor_receive);
  HAL_UART_Transmit(&huart1, (uint8_t *)"Booting up...\r\n", 14, 1000);
  uint8_t boot_sync_signal = 0x00;
  while (boot_sync_signal != 0xAA)
  {
      // Using a short timeout (100ms) so it repeatedly checks the UART interface
      HAL_UART_Receive(&huart1, &boot_sync_signal, 1, 100);
  }
  register_devices(sensor_array, 6);
  
  //receive ack after this untill proceeding further
  register_response(sensor_array, 6);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    telemtry_custom_send(&mysensor1);
    HAL_Delay(1000);
    telemtry_custom_send(&mysensor2);
    HAL_Delay(1000);
    telemtry_custom_send(&mysensor3);
    HAL_Delay(1000);
    telemtry_custom_send(&mysensor4);
    HAL_Delay(1000);
    telemtry_custom_send(&mysensor5);
    HAL_Delay(1000);
    telemtry_custom_send(&mysensor6);
    HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
