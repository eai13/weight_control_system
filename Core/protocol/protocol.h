#ifndef _DRIVE_PROTOCOL_H_
#define _DRIVE_PROTOCOL_H_

#include "global_config.h"
#include "drive_config.h"
#include "pid.h"

typedef enum{
    ERROR_CRC   = 0x01,
    ERROR_CMD   = 0x02,
    ERROR_ID    = 0x03,
    ERROR_READ  = 0x04,
    ERROR_WRITE = 0x05
}error_t;

typedef enum{
    ID_DRIVE_1      = 0x00,
    ID_DRIVE_2      = 0x01,
    ID_DRIVE_3      = 0x02,
    ID_DRIVE_4      = 0x03,
    ID_GLOBAL       = 0x04
}id_t;

typedef enum{
    CMD_WRITE_REG       = 0x01,
    CMD_READ_REG        = 0x02,
    CMD_BLOCK_DRIVE     = 0x03,
    CMD_ENABLE_DRIVE    = 0x05,
    CMD_RESET_SYSTEM    = 0x07,
    CMD_EXIT_APP        = 0x08,
    CMD_ERROR           = 0x09
}command_t;

typedef enum{
    REGISTER_TORQUE     = 0x00,
    REGISTER_POS_SP     = 0x01,    
    REGISTER_POS_FB     = 0x02,
    REGISTER_POS_ACC    = 0x03,
    REGISTER_POS_Kp     = 0x04,
    REGISTER_POS_Ki     = 0x05,
    REGISTER_POS_Kd     = 0x06,
    REGISTER_SPD_SP     = 0x07,
    REGISTER_SPD_FB     = 0x08,
    REGISTER_SPD_ACC    = 0x09,
    REGISTER_SPD_Kp     = 0x0A,
    REGISTER_SPD_Ki     = 0x0B,
    REGISTER_SPD_Kd     = 0x0C,
    REGISTER_CUR_SP     = 0x0D,
    REGISTER_CUR_FB     = 0x0E,
    REGISTER_CUR_ACC    = 0x0F,
    REGISTER_CUR_Kp     = 0x10,
    REGISTER_CUR_Ki     = 0x11,
    REGISTER_CUR_Kd     = 0x12,
    REGISTER_OUTPUT     = 0x13
}registers_t;

#pragma pack(push, 1)
typedef struct{
    id_t        id;
    command_t   cmd;
}frame_header_t;

typedef struct{
    float data;
    uint8_t crc8;
}single_reg_data_t;

typedef struct{
    float data[4];
    uint8_t crc8;
}multiple_reg_data_t;

typedef struct{
    uint8_t arg;
    uint8_t crc8;
}global_cmd_data_t;
#pragma pack(pop)

void PROTOCOL_ProcessFrame(void);

#endif