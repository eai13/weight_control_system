#ifndef _BOOTLOADER_CONFIG_H_
#define _BOOTLOADER_CONFIG_H_

#define BOOT_UART   huart4

#define DEV_NAME    "BOOT"

#define SECTORS_AMOUNT            12

#define APP_1_START_ADDRESS     (uint32_t)0x08020000
#define APP_1_START_SECTOR      5

#define APP_2_START_ADDRESS     (uint32_t)0x08060000
#define APP_2_START_SECTOR      7

#define APP_USER_START_ADDRESS  (uint32_t)0x080A0000
#define APP_USER_START_SECTOR   9

#define LAST_CALL_TIMEOUT       5000

#define _LED_ON_()          HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, 1)
#define _LED_OFF_()         HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, 0)

#define _LED_FAULT_ON_()    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 1)
#define _LED_FAULT_OFF_()   HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 0)

#endif