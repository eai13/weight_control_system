#ifndef _BOOTLOADER_CONFIG_H_
#define _BOOTLOADER_CONFIG_H_

#define BOOT_UART   huart1

#define DEV_NAME    "BOOT"

#define PAGES_AMOUNT            32

#define APP_1_START_ADDRESS     (uint32_t)0x08004000
#define APP_1_START_PAGE        8

#define APP_2_START_ADDRESS     (uint32_t)0x0800E000
#define APP_2_START_PAGE        28

#define APP_USER_START_ADDRESS  (uint32_t)0x08016000
#define APP_USER_START_PAGE     44

#define LAST_CALL_TIMEOUT       5000

#define _LED_ON_() HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1)
#define _LED_OFF_() HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0)

#endif