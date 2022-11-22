#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "stm32g0xx_hal.h"
#include <stdint.h>

#define MEMORY_I2C_HANDLE       hi2c1
#define MEMORY_MODULE_ADDRESS   0b10100000

#define MEMORY_NAME_ADDRESS     0x00
#define MEMORY_NAME             "MEMP"

#define POSITION_1_ADDRESS      0x10
#define POSITION_2_ADDRESS      0x14
#define POSITION_3_ADDRESS      0x18
#define POSITION_4_ADDRESS      0x1C

HAL_StatusTypeDef MEMORY_CheckMemoryName(uint32_t timeout);
HAL_StatusTypeDef MEMORY_PreparePartition(uint32_t timeout);

HAL_StatusTypeDef MEMORY_SetActualPosition(uint32_t pos1, uint32_t pos2, uint32_t pos3, uint32_t pos4, uint32_t timeout);
HAL_StatusTypeDef MEMORY_GetActualPosition(uint32_t * pos, uint32_t timeout);

#endif