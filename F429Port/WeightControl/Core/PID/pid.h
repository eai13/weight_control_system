#ifndef _PID_H_
#define _PID_H_

#include <stdint.h>
#include "stm32f429xx.h"

typedef enum{
    RO = 0x01,
    WO = 0x02,
    RW = 0x03
}rw_t;

typedef enum{
    RW_OK       = 0x01,
    RW_ERROR    = 0x02
}rw_status_t;

typedef struct{
    rw_t        p;
    uint16_t *  v;
}p_uint16_register_t;

typedef struct{
    rw_t        p;
    uint32_t *  v;
}p_uint32_register_t;

typedef struct{
    rw_t        p;
    float       v;
}float_register_t;

typedef struct{
    float_register_t sp;
    float_register_t fb;
    float_register_t acc;
    float_register_t acc_thres;
    float_register_t perr;
    float_register_t Kp;
    float_register_t Ki;
    float_register_t Kd;
    float_register_t isActive;
}loop_t;

typedef struct{
  GPIO_TypeDef *  frw_port;
  uint16_t        frw_pin;
  GPIO_TypeDef *  rev_port;
  uint16_t        rev_pin;
}direction_control_t;

typedef enum{
    DIR_STOP    = 0x00,
    DIR_FORWARD = 0x01,
    DIR_REVERSE = 0x02
}wc_drive_dir_e;

typedef struct{
    void * semaphore;
    uint32_t last_pid_call;
    union{
        struct{
            float_register_t    torque;
            loop_t              position_l;
            loop_t              speed_l;
            loop_t              current_l;
            float_register_t    output;
            float_register_t    output_thres;
        };
        float_register_t all_registers[30];
    };
    direction_control_t dir_pins;
    p_uint32_register_t pwm_duty;
    p_uint32_register_t encoder_s;
    p_uint16_register_t current_s;
    wc_drive_dir_e      direction;
}device_t;

#pragma pack(push, 1)
typedef struct{
    float pos_sp;
    float pos_fb;
    float spd_sp;
    float spd_fb;
    float cur_sp;
    float cur_fb;
    float output;
}wc_plottables_t;
#pragma pack(pop)

void PID_DriveCompute(uint8_t drive_num);

rw_status_t     PID_WriteReg(uint8_t drive_num, uint8_t reg, float data);
rw_status_t     PID_ReadReg(uint8_t drive_num, uint8_t reg, float * data);
wc_plottables_t PID_ReadPlottables(uint8_t drive_num);

void        PID_MoveSetpoints(void);
void        PID_StopDrive(uint8_t drive_num);
void        PID_SetZero(uint8_t drive_num);
float       PID_CalibEncoder(uint8_t drive_num, float value);

void PID_Task(void);

void PID_GetPositions(uint32_t * pos);

extern device_t drives[4];
extern uint16_t current[4];

#endif