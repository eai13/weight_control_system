#include "bootloader_config.h"
#include "bootloader.h"
#include "usart.h"
#include "main.h"
#include "stm32f4xx_hal_flash_ex.h"
#include <string.h>
#include <stdint.h>

#define SET_HEADER(s_cmd, s_size)  \
    header.cmd = s_cmd; \
    header.w_size = s_size;

#define P_ANS_ID        (payload[0])

#define J_REC_PARTITION (payload[0])

#define V_REC_PARTITION (payload[0])
#define V_REC_LENGTH    (payload[1])
#define V_ANS_CRC       (payload[0])

#define E_REC_PARTITION (payload[0])

#define W_REC_PARTITION (payload[0])
#define W_REC_LENGTH    (payload[1])
#define W_REC_CRC       (payload[2])
#define W_REC_DATA      (payload[3])

#define R_REC_PARTITION (payload[0])
#define R_REC_LENGTH    (payload[1])
#define R_ANS_CRC       (payload[0])
#define R_ANS_DATA      (payload[1])

uint8_t id[4] = DEV_NAME;

boot_partition_t partitions[3] = {
    {
        .address            = APP_1_START_ADDRESS,
        .sector             = APP_1_START_SECTOR,
        .sector_size        = APP_2_START_SECTOR - APP_1_START_SECTOR,
        .write_address      = APP_1_START_ADDRESS,
        .read_address       = APP_1_START_ADDRESS,
        .read_last_call     = 0,
        .write_last_call    = 0
    },
    {
        .address            = APP_2_START_ADDRESS,
        .sector             = APP_2_START_SECTOR,
        .sector_size        = APP_USER_START_SECTOR - APP_2_START_SECTOR,
        .write_address      = APP_2_START_ADDRESS,
        .read_address       = APP_2_START_ADDRESS,
        .read_last_call     = 0,
        .write_last_call    = 0
    },
    {
        .address            = APP_USER_START_ADDRESS,
        .sector             = APP_USER_START_SECTOR,
        .sector_size        = SECTORS_AMOUNT - APP_USER_START_SECTOR,
        .write_address      = APP_USER_START_ADDRESS,
        .read_address       = APP_USER_START_ADDRESS,
        .read_last_call     = 0,
        .write_last_call    = 0
    }
};

boot_header_t header = {
    .cmd = 0,
    .w_size = 0
};

uint32_t payload[256] = { 0 };

uint8_t raw_pack[1024] = { 0 };

uint8_t start_bytes_tx[2] = { 0xA5, 0x5A };
uint8_t start_bytes_rx[2];

void Bootloader_Process(void){
    if (HAL_UART_Receive(&BOOT_UART, start_bytes_rx, 2, 100) != HAL_OK) return;
    if ((start_bytes_rx[0] != 0xA5) || (start_bytes_rx[1] != 0x5A)) return;
    HAL_StatusTypeDef status = HAL_UART_Receive(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);
    if (status == HAL_OK){
        if (header.w_size){
            if (HAL_UART_Receive(&BOOT_UART, (uint8_t *)payload, header.w_size * 4, 100) != HAL_OK){
                SET_HEADER(ERROR_LENGTH, 0);
                HAL_UART_Transmit(&BOOT_UART, start_bytes_tx, 2, 10);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);
                return;
            }
        }

        if ((header.cmd <= CMD_FIRST) || (header.cmd >= CMD_LAST)){
            SET_HEADER(ERROR_CMD, 0);
            HAL_UART_Transmit(&BOOT_UART, start_bytes_tx, 2, 10);
            HAL_UART_Transmit(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);
        }

        switch(header.cmd){
            case(CMD_PING):
                C_Ping();
                HAL_UART_Transmit(&BOOT_UART, start_bytes_tx, 2, 10);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)payload, header.w_size * 4, 100);
                break;
            case(CMD_VERIFY):
                C_Verify(V_REC_PARTITION, V_REC_PARTITION);
                HAL_UART_Transmit(&BOOT_UART, start_bytes_tx, 2, 10);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)payload, header.w_size * 4, 100);
                break;
            case(CMD_JUMP):
                C_Jump(J_REC_PARTITION);
                HAL_UART_Transmit(&BOOT_UART, start_bytes_tx, 2, 10);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);
                break;
            case(CMD_READ):
                C_Read(R_REC_PARTITION, R_REC_LENGTH);
                HAL_UART_Transmit(&BOOT_UART, start_bytes_tx, 2, 10);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)payload, header.w_size * 4, 100);
                break;
            case(CMD_WRITE):
                C_Write(W_REC_PARTITION, W_REC_LENGTH, W_REC_CRC);
                HAL_UART_Transmit(&BOOT_UART, start_bytes_tx, 2, 10);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);
                break;
            case(CMD_ERASE):
                C_Erase(E_REC_PARTITION);
                HAL_UART_Transmit(&BOOT_UART, start_bytes_tx, 2, 10);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);
                break;
            default:
                SET_HEADER(ERROR_CMD, 0);
                HAL_UART_Transmit(&BOOT_UART, start_bytes_tx, 2, 10);
                HAL_UART_Transmit(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);
                break;
        }
    }
    else if (status != HAL_TIMEOUT){
        __NOP();
        __NOP();
    }
}

void   C_Ping(void){
    SET_HEADER(CMD_PING, 1);
    memcpy((void *)&P_ANS_ID, id, 4);
}

void   C_Jump(boot_part_e partition){
    if (partition >= PART_LAST){
        SET_HEADER(ERROR_PARTITION, 0);
        return;
    }

    SET_HEADER(CMD_JUMP, 0);
    HAL_UART_Transmit(&BOOT_UART, start_bytes_tx, 2, 10);
    HAL_UART_Transmit(&BOOT_UART, (uint8_t *)&header, sizeof(boot_header_t), 100);

    uint32_t p_jump_func_addr;
    void(* p_jump_func)(void);

    p_jump_func_addr = *(volatile uint32_t *)(partitions[partition].address + 4);
    p_jump_func = (void (*)(void))p_jump_func_addr;

    __disable_irq();
    SCB->VTOR = partitions[partition].address;
    // __set_MSP(*(volatile uint32_t *)partitions[partition].address);

    p_jump_func();
}

void   C_Verify(boot_part_e partition, uint32_t wlen){
    if (partition >= PART_LAST){
        SET_HEADER(ERROR_PARTITION, 0);
        return;
    }
    SET_HEADER(CMD_VERIFY, 1);
    V_ANS_CRC = 0;
}

void   C_Erase(boot_part_e partition){
    if (partition >= PART_LAST){
        SET_HEADER(ERROR_PARTITION, 0);
        return;
    }

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase_struct = {
        .TypeErase = TYPEERASE_SECTORS,
        .Sector = partitions[partition].sector,
        .NbSectors = partitions[partition].sector_size,
        .VoltageRange = VOLTAGE_RANGE_3
    };

    uint32_t erase_error = 0;
    if (HAL_FLASHEx_Erase(&erase_struct, &erase_error) != HAL_OK){
        SET_HEADER(ERROR_INTERNAL, 0);
        return;
    }

    HAL_FLASH_Lock();

    SET_HEADER(CMD_ERASE, 0);
}

void   C_Write(boot_part_e partition, uint32_t wlen, uint32_t crc32){
    if (partition >= PART_LAST){
        SET_HEADER(ERROR_PARTITION, 0);
        return;
    }
    if (/*check crc*/0){
        SET_HEADER(ERROR_CRC, 0);
        return;
    }

    if (HAL_GetTick() - partitions[partition].write_last_call > LAST_CALL_TIMEOUT)
        partitions[partition].write_address = partitions[partition].address;

    HAL_FLASH_Unlock();

    partitions[partition].write_last_call = HAL_GetTick();

    for (uint32_t iter = 0; iter < wlen; iter += 1){
        if (HAL_FLASH_Program(TYPEPROGRAM_WORD, partitions[partition].write_address, *(payload + 3 + iter)) != HAL_OK){
            HAL_FLASH_Lock();
            SET_HEADER(ERROR_INTERNAL, 0);
            return;
        }
        partitions[partition].write_address += 4;
    }

    HAL_FLASH_Lock();

    SET_HEADER(CMD_WRITE, 0);
}

void   C_Read(boot_part_e partition, uint32_t wlen){
    if (partition >= PART_LAST){
        SET_HEADER(ERROR_PARTITION, 0);
        return;
    }
    if (wlen > (partitions[partition].sector_size * 4)){
        SET_HEADER(ERROR_DATA, 0);
        return;
    }

    if (HAL_GetTick() - partitions[partition].read_last_call > LAST_CALL_TIMEOUT)
        partitions[partition].read_address = partitions[partition].address;

    partitions[partition].read_last_call = HAL_GetTick();

    for (uint32_t iter = 0; iter < wlen; iter++){
        payload[1 + iter] = *(volatile uint32_t *)(partitions[partition].read_address);
        partitions[partition].read_address += 4;
    }

    /*check crc*/R_ANS_CRC = 0;

    SET_HEADER(CMD_READ, wlen + 1);
}