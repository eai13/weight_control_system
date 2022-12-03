#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#include <stdint.h>
#include "bootloader_config.h"
#include "main.h"

typedef enum{
    CMD_FIRST,
    CMD_PING    = 0x01,
    CMD_VERIFY  = 0x02,
    CMD_JUMP    = 0x03,
    CMD_READ    = 0x04,
    CMD_WRITE   = 0x05,
    CMD_ERASE   = 0x06,
    CMD_LAST,
    ERROR_INTERNAL          = 0x08,
    ERROR_CRC               = 0x09,
    ERROR_CMD               = 0x0A,
    ERROR_PARTITION         = 0x0B,
    ERROR_LENGTH            = 0x0C,
    ERROR_DATA              = 0x0D
}boot_cmd_e;

typedef enum{
    PART_1      = 0x00,
    PART_2      = 0x01,
    PART_USER   = 0x02,
    PART_LAST
}boot_part_e;

#pragma pack(push, 1)
typedef struct{
    boot_cmd_e  cmd;
    uint32_t    w_size;
}boot_header_t;
#pragma pack(pop)

typedef struct{
    uint32_t address;
    uint32_t sector;
    uint32_t sector_size;
    uint32_t write_address;
    uint32_t read_address;
    uint32_t write_last_call;
    uint32_t read_last_call;
}boot_partition_t;

void   C_Ping(void);
void   C_Jump(boot_part_e partition);
void   C_Verify(boot_part_e partition, uint32_t wlen);
void   C_Erase(boot_part_e partition);
void   C_Write(boot_part_e partition, uint32_t wlen, uint32_t crc32);
void   C_Read(boot_part_e partition, uint32_t wlen);

void Bootloader_Process(void);

#endif