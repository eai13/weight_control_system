#ifndef _DRIVE_PROTOCOL_H_
#define _DRIVE_PROTOCOL_H_

#include "global_config.h"
#include "drive_config.h"
#include "pid.h"

#define SELF_ID "APP1"

typedef enum{
    HEADER_BASIC    = 0x01,
    HEADER_DATA     = 0x02
}protocol_header_e;

typedef enum{
    BP_CMD_PING             = 0x01,
    BP_CMD_JUMP             = 0x03,
    BP_ERROR_UNKNOWN_CMD    = 0xE0,
    BP_CMD_CONTROL          = 0xF0
}bp_cmd_e;

#pragma pack(push, 1)
typedef struct{
    bp_cmd_e    cmd;
    uint32_t    w_size;
}bp_header_t;
#pragma pack(pop)

typedef enum{
    ERROR_NO_ERROR          = 0x00,
    ERROR_CRC               = 0x01,
    ERROR_CMD               = 0x02,
    ERROR_ID                = 0x03,
    ERROR_READ              = 0x04,
    ERROR_WRITE             = 0x05,
    ERROR_INTERNAL_ERROR    = 0x06
}wc_errors_e;

typedef enum{
    ID_DRIVE_1      = 0x00,
    ID_DRIVE_2      = 0x01,
    ID_DRIVE_3      = 0x02,
    ID_DRIVE_4      = 0x03,
    ID_GLOBAL       = 0x04,
    ID_GLOBAL_CMD   = 0x05,
    ID_LAST
}wc_id_e;

typedef enum{
    CMD_WRITE_REG                   = 0x01,
    CMD_READ_REG                    = 0x02,
    CMD_BLOCK_DRIVE                 = 0x03,
    CMD_ENABLE_DRIVE                = 0x05,
    CMD_READ_PLOTTABLE              = 0x06,
    CMD_RESET_SYSTEM                = 0x07,
    CMD_EXIT_APP                    = 0x08,
    CMD_ERROR                       = 0x09,
    CMD_CALIBRATE_AS_ZERO_POSITION  = 0x0A,
    CMD_STOP_DRIVE                  = 0x0B
}wc_commands_e;

typedef enum{
    REGISTER_TORQUE         = 0x00,
    REGISTER_POS_SP         = 0x01,    
    REGISTER_POS_FB         = 0x02,
    REGISTER_POS_ACC        = 0x03,
    REGISTER_POS_ACC_THRES  = 0x04,
    REGISTER_POS_PERR       = 0x05,
    REGISTER_POS_Kp         = 0x06,
    REGISTER_POS_Ki         = 0x07,
    REGISTER_POS_Kd         = 0x08,
    REGISTER_POS_IS_ACT     = 0x09,
    REGISTER_SPD_SP         = 0x0A,
    REGISTER_SPD_FB         = 0x0B,
    REGISTER_SPD_ACC        = 0x0C,
    REGISTER_SPD_ACC_THRES  = 0x0D,
    REGISTER_SPD_PERR       = 0x0E,
    REGISTER_SPD_Kp         = 0x0F,
    REGISTER_SPD_Ki         = 0x10,
    REGISTER_SPD_Kd         = 0x11,
    REGISTER_SPD_IS_ACT     = 0x12,
    REGISTER_CUR_SP         = 0x13,
    REGISTER_CUR_FB         = 0x14,
    REGISTER_CUR_ACC        = 0x15,
    REGISTER_CUR_ACC_THRES  = 0x16,
    REGISTER_CUR_PERR       = 0x17,
    REGISTER_CUR_Kp         = 0x18,
    REGISTER_CUR_Ki         = 0x19,
    REGISTER_CUR_Kd         = 0x1A,
    REGISTER_CUR_IS_ACT     = 0x1B,
    REGISTER_OUTPUT         = 0x1C,
    REGISTER_OUTPUT_THRES   = 0x1D,
    REGISTER_LAST
}wc_registers_e;

#pragma pack(push, 1)
typedef struct{
    uint32_t        id;
    uint32_t        cmd;
}control_header_t;

typedef struct{
    uint32_t        reg;
    float           data;
}single_reg_data_t;

typedef struct{
    uint32_t        reg;
    float           data[4];
}multiple_reg_data_t;

typedef struct{
    uint32_t        arg;
}global_cmd_data_t;

typedef struct{
    uint32_t        id;
    uint32_t        status;
}response_header_t;
#pragma pack(pop)

uint8_t PROTOCOL_MessagePending(void);
void    PROTOCOL_ResetPendingFlag(void);

void PROTOCOL_ProcessFrame(void);
void PROTOCOL_Start(void);

#endif