#ifndef _BOOTLOADER_CONFIG_H_
#define _BOOTLOADER_CONFIG_H_

#define BOOT_UART   huart1

#define DEV_NAME    "EGOR"

#define PAGES_AMOUNT            64

#define APP_1_START_ADDRESS     (uint32_t)0x08006000
#define APP_1_START_PAGE        12

#define APP_2_START_ADDRESS     (uint32_t)0x0800D800
#define APP_2_START_PAGE        27

#define APP_USER_START_ADDRESS  (uint32_t)0x08015000
#define APP_USER_START_PAGE     42

#define LAST_CALL_TIMEOUT       1000

#define _LED_ON_() HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1)
#define _LED_OFF_() HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0)

#endif