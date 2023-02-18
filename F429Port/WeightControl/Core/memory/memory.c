#include "memory.h"
#include "stm32f4xx_hal_i2c.h"
#include "i2c.h"
#include "string.h"
#include <stdint.h>
#include "main.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pid.h"

HAL_StatusTypeDef MEMORY_CheckMemoryName(uint32_t timeout){
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t name[4];
    status = HAL_I2C_Mem_Read(&MEMORY_I2C_HANDLE,
                              MEMORY_MODULE_ADDRESS,
                              MEMORY_NAME_ADDRESS,
                              I2C_MEMADD_SIZE_8BIT,
                              name, 4, timeout);
    // print_in("Memory Name is : %.4s\r\n", name);
    if (status != HAL_OK) return status;
    if (strncmp(name, MEMORY_NAME, 4) != 0){
        // print_er("Names not equal\r\n");
        return HAL_ERROR;
    }
    return status;
}
HAL_StatusTypeDef MEMORY_PreparePartition(uint32_t timeout){
    HAL_StatusTypeDef status = HAL_OK;
    status = HAL_I2C_Mem_Write(&MEMORY_I2C_HANDLE,
                               MEMORY_MODULE_ADDRESS,
                               MEMORY_NAME_ADDRESS,
                               I2C_MEMADD_SIZE_8BIT,
                               MEMORY_NAME, 4, timeout);
    if (status != HAL_OK) return status;
    uint32_t ticks_count = HAL_GetTick();
    while(HAL_I2C_IsDeviceReady(&MEMORY_I2C_HANDLE,
                                MEMORY_MODULE_ADDRESS,
                                1, HAL_MAX_DELAY) != HAL_OK){
        if (HAL_GetTick() - ticks_count > timeout){
            break;
        } 
    }
    return status;
}
HAL_StatusTypeDef MEMORY_SetActualPosition(uint32_t pos1, uint32_t pos2, uint32_t pos3, uint32_t pos4, uint32_t timeout){
    uint32_t positions[4] = {
        pos1, pos2, pos3, pos4
    };
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&MEMORY_I2C_HANDLE,
                                                 MEMORY_MODULE_ADDRESS,
                                                 POSITION_1_ADDRESS,
                                                 I2C_MEMADD_SIZE_8BIT,
                                                 positions, 16, timeout);
    if (status != HAL_OK) return status;
    uint32_t ticks_count = HAL_GetTick();
    while(HAL_I2C_IsDeviceReady(&MEMORY_I2C_HANDLE,
                                MEMORY_MODULE_ADDRESS,
                                1, HAL_MAX_DELAY) != HAL_OK){
        if (HAL_GetTick() - ticks_count > timeout){
            break;
        } 
    }
    return status;
}
HAL_StatusTypeDef MEMORY_GetActualPosition(uint32_t * pos, uint32_t timeout){
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&MEMORY_I2C_HANDLE,
                                                MEMORY_MODULE_ADDRESS,
                                                POSITION_1_ADDRESS,
                                                I2C_MEMADD_SIZE_8BIT,
                                                pos, 16, timeout);
    return status;
}

void MEMORY_SaveAndExit(void){
    uint32_t positions_to_save[4];
    PID_GetPositions(positions_to_save);
    taskENTER_CRITICAL();
    MEMORY_SetActualPosition(positions_to_save[0],
                             positions_to_save[1],
                             positions_to_save[2],
                             positions_to_save[3], 100);
    HAL_NVIC_SystemReset();
}

void MEMORY_Task(void){
    
    uint32_t positions_to_save[4];

    while(1){

        PID_GetPositions(positions_to_save);
        MEMORY_SetActualPosition(positions_to_save[0],
                                 positions_to_save[1],
                                 positions_to_save[2],
                                 positions_to_save[3], 100);
        LED_CONTROL_TOGGLE();
        osDelay(3000);
        
    }
}