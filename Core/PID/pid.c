#include "pid.h"
#include "pid_config.h"
#include "global_config.h"
#include "drive_config.h"
#include "stm32g071xx.h"
#include "main.h"
#include <math.h>
#include <stdint.h>
#include "debug.h"

#include "lptim.h"

// #ifdef _DEBUG_VERSION_
// #include "debug.h"
// #endif

#define DRIVE   (drives[drive_num])
#define _ABS_FLOAT(value) \
            ((((float)(value)) > ((float)(0.0))) ? ((float)(value)) : (((float)(-1)) * ((float)(value))))

uint16_t current[4] = { 0, 0, 0, 0 };
float    set_position[4] = { 0, 0, 0, 0 };
uint32_t lptim_corrector = 0;

void PID_SetNewCorrector(uint32_t corrector){
    lptim_corrector = corrector;
}
uint32_t PID_GetCorrector(void){
    return lptim_corrector;
}

// TMP
uint32_t int_count = 0x7FFFFFFF;
uint32_t GetIntCount(){
    return int_count;
}
// TMP

void HAL_LPTIM_AutoReloadMatchCallback(LPTIM_HandleTypeDef *hlptim){
    uint16_t tmp = ENCODER_4_COUNT;
  if (hlptim == &ENCODER_4_TIMER){
    // if (tmp == 0) return;
    if (tmp < 0x7FFF){
      int_count++;
      lptim_corrector += 0xFFFF;
    }
    else{
      int_count--;
      lptim_corrector -= 0xFFFF;
    }
  }
}

device_t drives[4] = {
    { // DRIVE 1
        .torque = { RW, 0 },
        .position_l = { 
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_POS_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_POSITION_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_POSITION_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_POSITION_Kd },
            .isActive   = { RW, 100 }
        },
        .speed_l = {
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_SPEED_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_SPEED_Kp},
            .Ki         = { RW, WC_CONFIG_DEFAULT_SPEED_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_SPEED_Kd },
            .isActive   = { RW, 100 }
        },
        .current_l = {
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_CURR_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_CURRENT_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_CURRENT_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_CURRENT_Kd },
            .isActive   = { RW, -100 }
        },
        .output         = { RO, 0 },
        .output_thres   = { RW, PWM_UPPER_COUNT / 2 },
        .encoder_s      = { RW, &ENCODER_1_COUNT },
        .current_s      = { RO, &(current[0]) },
        .pwm_duty       = { RO, &PWM_1_DUTY },
        .dir_pins       = {
            .frw_port   = DRV1_DIR_0_GPIO_Port,
            .frw_pin    = DRV1_DIR_0_Pin,
            .rev_port   = DRV1_DIR_1_GPIO_Port,
            .rev_pin    = DRV1_DIR_1_Pin
        },
        .direction      = DIR_STOP
    },
    { // DRIVE 2
        .torque = { RW, 0 },
        .position_l = { 
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_POS_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_POSITION_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_POSITION_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_POSITION_Kd },
            .isActive   = { RW, 100 }
        },
        .speed_l = {
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_SPEED_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_SPEED_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_SPEED_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_SPEED_Kd },
            .isActive   = { RW, 100 }
        },
        .current_l = {
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_CURR_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_CURRENT_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_CURRENT_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_CURRENT_Kd },
            .isActive   = { RW, -100 }
        },
        .output         = { RO, 0 },
        .output_thres   = { RW, PWM_UPPER_COUNT / 2 },
        .encoder_s      = { RW, &ENCODER_2_COUNT },
        .current_s      = { RO, &(current[1]) },
        .pwm_duty       = { RO, &PWM_2_DUTY },
        .dir_pins       = {
            .frw_port   = DRV2_DIR_0_GPIO_Port,
            .frw_pin    = DRV2_DIR_0_Pin,
            .rev_port   = DRV2_DIR_1_GPIO_Port,
            .rev_pin    = DRV2_DIR_1_Pin
        },
        .direction      = DIR_STOP
    },
    { // DRIVE 3
        .torque = { RW, 0 },
        .position_l = { 
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_POS_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_POSITION_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_POSITION_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_POSITION_Kd },
            .isActive   = { RW, 100 }
        },
        .speed_l = {
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_SPEED_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_SPEED_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_SPEED_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_SPEED_Kd },
            .isActive   = { RW, 100 }
        },
        .current_l = {
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_CURR_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_CURRENT_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_CURRENT_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_CURRENT_Kd },
            .isActive   = { RW, -100 }
        },
        .output         = { RO, 0 },
        .output_thres   = { RW, PWM_UPPER_COUNT / 2 },
        .encoder_s      = { RW, &ENCODER_3_COUNT },
        .current_s      = { RO, &(current[2]) },
        .pwm_duty       = { RO, &PWM_3_DUTY },
        .dir_pins       = {
            .frw_port   = DRV3_DIR_0_GPIO_Port,
            .frw_pin    = DRV3_DIR_0_Pin,
            .rev_port   = DRV3_DIR_1_GPIO_Port,
            .rev_pin    = DRV3_DIR_1_Pin
        },
        .direction      = DIR_STOP
    },
    { // DRIVE 4
        .torque = { RW, 0 },
        .position_l = { 
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_POS_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_POSITION_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_POSITION_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_POSITION_Kd },
            .isActive   = { RW, 100 }
        },
        .speed_l = {
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_SPEED_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_SPEED_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_SPEED_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_SPEED_Kd },
            .isActive   = { RW, 100 }
        },
        .current_l = {
            .sp         = { RW, 0 },
            .fb         = { RO, 0 },
            .acc        = { RO, 0 },
            .acc_thres  = { RW, WC_CONFIG_DEFAULT_CURR_ACC_THRES },
            .perr       = { RO, 0 },
            .Kp         = { RW, WC_CONFIG_DEFAULT_CURRENT_Kp },
            .Ki         = { RW, WC_CONFIG_DEFAULT_CURRENT_Ki },
            .Kd         = { RW, WC_CONFIG_DEFAULT_CURRENT_Kd },
            .isActive   = { RW, -100 }
        },
        .output         = { RO, 0 },
        .output_thres   = { RW, PWM_UPPER_COUNT / 2 },
        .encoder_s      = { RW, &ENCODER_4_COUNT },
        .current_s      = { RO, &(current[3]) },
        .pwm_duty       = { RO, &PWM_4_DUTY },
        .dir_pins       = {
            .frw_port   = DRV4_DIR_0_GPIO_Port,
            .frw_pin    = DRV4_DIR_0_Pin,
            .rev_port   = DRV4_DIR_1_GPIO_Port,
            .rev_pin    = DRV4_DIR_1_Pin
        },
        .direction      = DIR_STOP
    }
};

// TMP
uint32_t GetMot4Count(void){
  return *(drives[3].encoder_s.v);  
}
// TMP

static inline
float GetRealCurrent(uint16_t raw){
    return (((float)(abs((int16_t)(raw)) - ADC_CURRENT_0)) / ADC_RESOLUTION * ADC_REFERENCE / ADC_VPA);
}

static inline
float GetRealRadial(int32_t raw){
    return (((float)(raw)) / ((float)(600.0)) * ((float)(6.28)) / ((float)(4.0)));
}

static inline
uint32_t VoltsToPWM(float volts){
    return ((int32_t)((volts) / ((float)(24.0)) * ((float)(PWM_UPPER_COUNT))));
}

static inline
void DriveForward(uint8_t drive_num){
    if (DRIVE.direction != DIR_FORWARD) DRIVE.direction = DIR_FORWARD;
    else return;
    HAL_GPIO_WritePin(DRIVE.dir_pins.rev_port, DRIVE.dir_pins.rev_pin, 0);
    for (uint32_t iter = 0; iter < 100; iter++){
        __NOP();
    }
    HAL_GPIO_WritePin(DRIVE.dir_pins.frw_port, DRIVE.dir_pins.frw_pin, 1);
}

static inline
void DriveReverse(uint8_t drive_num){
    if (DRIVE.direction != DIR_REVERSE) DRIVE.direction = DIR_REVERSE;
    else return;
    HAL_GPIO_WritePin(DRIVE.dir_pins.frw_port, DRIVE.dir_pins.frw_pin, 0);
    for (uint32_t iter = 0; iter < 100; iter++){
        __NOP();
    }
    HAL_GPIO_WritePin(DRIVE.dir_pins.rev_port, DRIVE.dir_pins.rev_pin, 1);
}

// uint8_t pid_enabled = 0;
// void SetPIDEnabled(void) { pid_enabled = 1; }
void PID_DriveCompute(uint8_t drive_num){
    // if (pid_enabled == 0) return;
    // // GETTING RADIAL POSITION
    float tmp_position = 0;
    if (drive_num == 3){
        tmp_position = GetRealRadial(((int32_t)(*(DRIVE.encoder_s.v))) + lptim_corrector - 0x3FFFFFFF);
    }
    else if (drive_num == 0){
        tmp_position = GetRealRadial(((int64_t)(*(DRIVE.encoder_s.v))) - 0x7FFFFFFF);
    }
    else{
        tmp_position = GetRealRadial(((int32_t)(*(DRIVE.encoder_s.v))) - 0x7FFF);
    }
    
    // FEEDBACK COMPUTE
    DRIVE.speed_l.fb.v      = 
            (tmp_position - DRIVE.position_l.fb.v) / TIME_CONSTANT;
    DRIVE.position_l.fb.v   =
            tmp_position;
    DRIVE.current_l.fb.v    =
            GetRealCurrent(*(DRIVE.current_s.v));

    // Position loop compute
    if (DRIVE.position_l.isActive.v > 0){
        if (DRIVE.position_l.Ki.v){
            DRIVE.position_l.acc.v +=
                    (DRIVE.position_l.sp.v - DRIVE.position_l.fb.v) * DRIVE.position_l.Ki.v;
            if (DRIVE.position_l.acc.v > DRIVE.position_l.acc_thres.v) DRIVE.position_l.acc.v = DRIVE.position_l.acc_thres.v;
            else if (DRIVE.position_l.acc.v < -DRIVE.position_l.acc_thres.v) DRIVE.position_l.acc.v = -DRIVE.position_l.acc_thres.v;
        }
        float d_part = 0;
        if (DRIVE.position_l.Kd.v){
            float nerr = DRIVE.position_l.sp.v - DRIVE.position_l.fb.v;
            d_part = (nerr - DRIVE.position_l.perr.v) / TIME_CONSTANT;
            DRIVE.position_l.perr.v = nerr;
        }
        DRIVE.speed_l.sp.v = 
                (DRIVE.position_l.sp.v - DRIVE.position_l.fb.v) * DRIVE.position_l.Kp.v + DRIVE.position_l.acc.v + d_part;
    }
    else{
        DRIVE.speed_l.sp.v = DRIVE.position_l.sp.v;
    }
    
    // Speed loop compute
    if (DRIVE.speed_l.isActive.v > 0){
        if (DRIVE.speed_l.Ki.v){
            DRIVE.speed_l.acc.v += 
                    (DRIVE.speed_l.sp.v - DRIVE.speed_l.fb.v) * DRIVE.speed_l.Ki.v;
            if (DRIVE.speed_l.acc.v > DRIVE.speed_l.acc_thres.v) DRIVE.speed_l.acc.v = DRIVE.speed_l.acc_thres.v;
            else if (DRIVE.speed_l.acc.v < -DRIVE.speed_l.acc_thres.v) DRIVE.speed_l.acc.v = -DRIVE.speed_l.acc_thres.v;
        }
        float d_part = 0;
        if (DRIVE.speed_l.Kd.v){
            float nerr = DRIVE.speed_l.sp.v - DRIVE.speed_l.fb.v;
            d_part = (nerr - DRIVE.speed_l.perr.v) / TIME_CONSTANT;
            DRIVE.speed_l.perr.v = nerr;
        }
        DRIVE.current_l.sp.v = 
                (DRIVE.speed_l.sp.v - DRIVE.speed_l.fb.v) * DRIVE.speed_l.Kp.v + DRIVE.speed_l.acc.v + d_part;
    }
    else{
        DRIVE.current_l.sp.v = DRIVE.speed_l.sp.v;
    }

    // Current loop compute
    if (DRIVE.current_l.isActive.v > 0){
        if (DRIVE.current_l.Ki.v){
            DRIVE.current_l.acc.v += 
                    (DRIVE.current_l.sp.v / DRIVE_REDUCTION + DRIVE.torque.v - DRIVE.current_l.fb.v) * DRIVE.current_l.Ki.v;
            if (DRIVE.current_l.acc.v > DRIVE.current_l.acc_thres.v) DRIVE.current_l.acc.v = DRIVE.current_l.acc_thres.v;
            else if (DRIVE.current_l.acc.v < -DRIVE.current_l.acc_thres.v) DRIVE.current_l.acc.v = -DRIVE.current_l.acc_thres.v;
        }
        float d_part = 0;
        if (DRIVE.current_l.Kd.v){
            float nerr = DRIVE.current_l.sp.v - DRIVE.current_l.fb.v;
            d_part = (nerr - DRIVE.current_l.perr.v) / TIME_CONSTANT;
            DRIVE.current_l.perr.v = nerr;
        }
        DRIVE.output.v = 
                (DRIVE.current_l.sp.v / DRIVE_REDUCTION + DRIVE.torque.v - DRIVE.current_l.fb.v) * DRIVE.current_l.Kp.v + DRIVE.current_l.acc.v + d_part;
    }
    else{
        DRIVE.output.v = DRIVE.current_l.sp.v / DRIVE_REDUCTION;
    }

    // OUTPUT COMPUTE
    if (DRIVE.output.v > ((float)(24.0))) DRIVE.output.v = ((float)(24.0));
    else if (DRIVE.output.v < ((float)(-24.0))) DRIVE.output.v = ((float)(-24.0));

    // SET DIRECTION
    if (DRIVE.output.v > 0) DriveForward(drive_num);
    else DriveReverse(drive_num);

    // CONVERT TO PWM
    uint16_t pwm_out = VoltsToPWM(_ABS_FLOAT(DRIVE.output.v));
    
    if (pwm_out > DRIVE.output_thres.v)
        *(DRIVE.pwm_duty.v) = DRIVE.output_thres.v;
    else
        *(DRIVE.pwm_duty.v) = pwm_out;
}

rw_status_t PID_WriteReg(uint8_t drive_num, uint8_t reg, float data){
    if (DRIVE.all_registers[reg].p & WO){
        DRIVE.all_registers[reg].v = data;
        return RW_OK;
    }
    else return RW_ERROR;
}

rw_status_t PID_ReadReg(uint8_t drive_num, uint8_t reg, float * data){
    if (DRIVE.all_registers[reg].p & RO){
        *data = DRIVE.all_registers[reg].v;
        return RW_OK;
    }
    else return RW_ERROR;
}

wc_plottables_t PID_ReadPlottables(uint8_t drive_num){
    wc_plottables_t tmp = { 0 };
    if (drive_num >= 4) return tmp; 
    tmp.pos_sp = DRIVE.position_l.sp.v;
    tmp.pos_fb = DRIVE.position_l.fb.v;
    tmp.spd_sp = DRIVE.speed_l.sp.v;
    tmp.spd_fb = DRIVE.speed_l.fb.v;
    tmp.cur_sp = DRIVE.current_l.sp.v;
    tmp.cur_fb = DRIVE.current_l.fb.v;
    tmp.output = DRIVE.output.v;
    return tmp;
}

void PID_MoveSetpoints(void){
    drives[0].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[0].encoder_s.v))) - 0x7FFFFFFF);
    drives[1].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[1].encoder_s.v))) - 0x7FFF);
    drives[2].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[2].encoder_s.v))) - 0x7FFF);
    drives[3].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[3].encoder_s.v))) + lptim_corrector - 0x3FFFFFFF);
}

void PID_SetZero(uint8_t drive_num){
    if (drive_num > 3){
        ENCODER_1_COUNT = 0x7FFFFFFF;
        drives[0].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[0].encoder_s.v))) - 0x7FFFFFFF);
        ENCODER_2_COUNT = 0x7FFF;
        drives[1].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[1].encoder_s.v))) - 0x7FFF);
        ENCODER_3_COUNT = 0x7FFF;
        drives[2].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[2].encoder_s.v))) - 0x7FFF);
        lptim_corrector = 0x3FFFFFFF - ENCODER_4_COUNT;
        drives[3].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[3].encoder_s.v))) + lptim_corrector - 0x3FFFFFFF);
    }
    else{
        switch(drive_num){
            case(0):{
                ENCODER_1_COUNT = 0x7FFFFFFF;
                drives[0].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[0].encoder_s.v))) - 0x7FFFFFFF);
                break;
            }
            case(1):{
                ENCODER_2_COUNT = 0x7FFF;
                drives[1].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[1].encoder_s.v))) - 0x7FFF);
                break;
            }
            case(2):{
                ENCODER_3_COUNT = 0x7FFF;
                drives[2].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[2].encoder_s.v))) - 0x7FFF);
                break;
            }
            case(3):{
                lptim_corrector = 0x3FFFFFFF - ENCODER_4_COUNT;
                drives[3].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[3].encoder_s.v))) + lptim_corrector - 0x3FFFFFFF);
                break;
            }
            default:{
                break;
            }
        }
    }
}

void PID_StopDrive(uint8_t drive_num){
    if (drive_num > 3){
        drives[0].position_l.sp.v = drives[0].position_l.fb.v;
        drives[1].position_l.sp.v = drives[1].position_l.fb.v;
        drives[2].position_l.sp.v = drives[2].position_l.fb.v;
        drives[3].position_l.sp.v = drives[3].position_l.fb.v;
    }
    else{
        DRIVE.position_l.sp.v = DRIVE.position_l.fb.v;
    }
}