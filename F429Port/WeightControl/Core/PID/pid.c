#include "pid.h"
#include "pid_config.h"
#include "global_config.h"
#include "drive_config.h"
#include "stm32f429xx.h"
#include "main.h"
#include <math.h>
#include <stdint.h>
#include "cmsis_os.h"

#include "macros.h"

#define DRIVE   (drives[drive_num])
#define _ABS_FLOAT(value) \
            ((((float)(value)) > ((float)(0.0))) ? ((float)(value)) : (((float)(-1)) * ((float)(value))))

uint16_t current[4] = { 0, 0, 0, 0 };

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

static inline
float GetRealCurrent(uint16_t raw){
    return (((float)(abs((int16_t)(raw)) - ADC_CURRENT_0)) / ADC_RESOLUTION * ADC_REFERENCE / ADC_VPA);
}

static inline
float GetRealRadial(int32_t raw){
    return (((float)(raw)) / ((float)(600.0)) * ((float)(6.28)) / ((float)(4.0)));
}

static inline
uint16_t GetEncoderFromRadial(float value){
    int32_t tmp_val = (int32_t)(value * ((float)(4.0)) / ((float)(6.28)) * ((float)(600.0)));
    tmp_val += (int32_t)(0x7FFF);
    return (uint16_t)(tmp_val);
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
    for (uint32_t iter = 0; iter < 300; iter++){
        __NOP();
    }
    HAL_GPIO_WritePin(DRIVE.dir_pins.frw_port, DRIVE.dir_pins.frw_pin, 1);
}

static inline
void DriveReverse(uint8_t drive_num){
    if (DRIVE.direction != DIR_REVERSE) DRIVE.direction = DIR_REVERSE;
    else return;
    HAL_GPIO_WritePin(DRIVE.dir_pins.frw_port, DRIVE.dir_pins.frw_pin, 0);
    for (uint32_t iter = 0; iter < 300; iter++){
        __NOP();
    }
    HAL_GPIO_WritePin(DRIVE.dir_pins.rev_port, DRIVE.dir_pins.rev_pin, 1);
}

void PID_DriveCompute(uint8_t drive_num){
    if (osSemaphoreAcquire(DRIVE.semaphore, 100) != osOK) return;
    // GETTING RADIAL POSITION
    float tmp_position = GetRealRadial(((int32_t)(*(DRIVE.encoder_s.v))) - 0x7FFF);
    
    float pid_period = ((float)(osKernelGetTickCount() - DRIVE.last_pid_call)) / (float)(1000);

    // FEEDBACK COMPUTE
    DRIVE.speed_l.fb.v      = 
            (tmp_position - DRIVE.position_l.fb.v) / pid_period;
    DRIVE.position_l.fb.v   =
            tmp_position;
    DRIVE.current_l.fb.v    =
            GetRealCurrent(*(DRIVE.current_s.v));

    // Position loop compute
    if (DRIVE.position_l.isActive.v > 0){
        if (DRIVE.position_l.Ki.v){
            DRIVE.position_l.acc.v +=
                    (DRIVE.position_l.sp.v - DRIVE.position_l.fb.v) * DRIVE.position_l.Ki.v * pid_period;
            if (DRIVE.position_l.acc.v > DRIVE.position_l.acc_thres.v) DRIVE.position_l.acc.v = DRIVE.position_l.acc_thres.v;
            else if (DRIVE.position_l.acc.v < -DRIVE.position_l.acc_thres.v) DRIVE.position_l.acc.v = -DRIVE.position_l.acc_thres.v;
        }
        float d_part = 0;
        if (DRIVE.position_l.Kd.v){
            float nerr = DRIVE.position_l.sp.v - DRIVE.position_l.fb.v;
            d_part = (nerr - DRIVE.position_l.perr.v) / pid_period;
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
                    (DRIVE.speed_l.sp.v - DRIVE.speed_l.fb.v) * DRIVE.speed_l.Ki.v * pid_period;
            if (DRIVE.speed_l.acc.v > DRIVE.speed_l.acc_thres.v) DRIVE.speed_l.acc.v = DRIVE.speed_l.acc_thres.v;
            else if (DRIVE.speed_l.acc.v < -DRIVE.speed_l.acc_thres.v) DRIVE.speed_l.acc.v = -DRIVE.speed_l.acc_thres.v;
        }
        float d_part = 0;
        if (DRIVE.speed_l.Kd.v){
            float nerr = DRIVE.speed_l.sp.v - DRIVE.speed_l.fb.v;
            d_part = (nerr - DRIVE.speed_l.perr.v) / pid_period;
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
                    (DRIVE.current_l.sp.v / DRIVE_REDUCTION + DRIVE.torque.v - DRIVE.current_l.fb.v) * DRIVE.current_l.Ki.v * pid_period;
            if (DRIVE.current_l.acc.v > DRIVE.current_l.acc_thres.v) DRIVE.current_l.acc.v = DRIVE.current_l.acc_thres.v;
            else if (DRIVE.current_l.acc.v < -DRIVE.current_l.acc_thres.v) DRIVE.current_l.acc.v = -DRIVE.current_l.acc_thres.v;
        }
        float d_part = 0;
        if (DRIVE.current_l.Kd.v){
            float nerr = DRIVE.current_l.sp.v - DRIVE.current_l.fb.v;
            d_part = (nerr - DRIVE.current_l.perr.v) / pid_period;
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

    osSemaphoreRelease(DRIVE.semaphore);
}

rw_status_t PID_WriteReg(uint8_t drive_num, uint8_t reg, float data){
    if (osSemaphoreAcquire(DRIVE.semaphore, 100) != osOK) return RW_ERROR;
    if (DRIVE.all_registers[reg].p & WO){
        DRIVE.all_registers[reg].v = data;
        osSemaphoreRelease(DRIVE.semaphore);
        return RW_OK;
    }
    else{
        osSemaphoreRelease(DRIVE.semaphore);
        return RW_ERROR;
    }
}

rw_status_t PID_ReadReg(uint8_t drive_num, uint8_t reg, float * data){
    if (osSemaphoreAcquire(DRIVE.semaphore, 100) != osOK) return RW_ERROR;
    if (DRIVE.all_registers[reg].p & RO){
        *data = DRIVE.all_registers[reg].v;
        osSemaphoreRelease(DRIVE.semaphore);
        return RW_OK;
    }
    else{
        osSemaphoreRelease(DRIVE.semaphore);
        return RW_ERROR;
    }
}

wc_plottables_t PID_ReadPlottables(uint8_t drive_num){
    wc_plottables_t tmp = { 0 };
    if (osSemaphoreAcquire(DRIVE.semaphore, 100) != osOK) return tmp;
    if (drive_num >= 4){
        osSemaphoreRelease(DRIVE.semaphore);
        return tmp;
    } 
    tmp.pos_sp = DRIVE.position_l.sp.v;
    tmp.pos_fb = DRIVE.position_l.fb.v;
    tmp.spd_sp = DRIVE.speed_l.sp.v;
    tmp.spd_fb = DRIVE.speed_l.fb.v;
    tmp.cur_sp = DRIVE.current_l.sp.v;
    tmp.cur_fb = DRIVE.current_l.fb.v;
    tmp.output = DRIVE.output.v;
    osSemaphoreRelease(DRIVE.semaphore);
    return tmp;
}

void PID_MoveSetpoints(void){
    if (osSemaphoreAcquire(drives[0].semaphore, 100) == osOK){
        drives[0].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[0].encoder_s.v))) - 0x7FFF);
        osSemaphoreRelease(drives[0].semaphore);
    }
    if (osSemaphoreAcquire(drives[1].semaphore, 100) == osOK){
        drives[1].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[1].encoder_s.v))) - 0x7FFF);
        osSemaphoreRelease(drives[1].semaphore);
    }
    if (osSemaphoreAcquire(drives[2].semaphore, 100) == osOK){
        drives[2].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[2].encoder_s.v))) - 0x7FFF);
        osSemaphoreRelease(drives[2].semaphore);
    }
    if (osSemaphoreAcquire(drives[3].semaphore, 100) == osOK){
        drives[3].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[3].encoder_s.v))) - 0x7FFF);
        osSemaphoreRelease(drives[3].semaphore);
    }
}

void PID_SetZero(uint8_t drive_num){
    if (drive_num > 3){
        if (osSemaphoreAcquire(drives[0].semaphore, 100) == osOK){
            ENCODER_1_COUNT = 0x7FFF;
            drives[0].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[0].encoder_s.v))) - 0x7FFF);
            osSemaphoreRelease(drives[0].semaphore);
        }
        if (osSemaphoreAcquire(drives[1].semaphore, 100) == osOK){
            ENCODER_2_COUNT = 0x7FFF;
            drives[1].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[1].encoder_s.v))) - 0x7FFF);
            osSemaphoreRelease(drives[1].semaphore);
        }
        if (osSemaphoreAcquire(drives[2].semaphore, 100) == osOK){
            ENCODER_3_COUNT = 0x7FFF;
            drives[2].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[2].encoder_s.v))) - 0x7FFF);
            osSemaphoreRelease(drives[2].semaphore);
        }
        if (osSemaphoreAcquire(drives[3].semaphore, 100) == osOK){
            ENCODER_4_COUNT = 0x7FFF;
            drives[3].position_l.sp.v = GetRealRadial(((int32_t)(*(drives[3].encoder_s.v))) - 0x7FFF);
            osSemaphoreRelease(drives[3].semaphore);
        }
    }
    else{
        if (osSemaphoreAcquire(DRIVE.semaphore, 100) == osOK){
            (*(DRIVE.encoder_s.v)) = 0x7FFF;
            DRIVE.position_l.sp.v = GetRealRadial(((int32_t)(*(DRIVE.encoder_s.v))) - 0x7FFF);
            osSemaphoreRelease(DRIVE.semaphore);
        }
    }
}

float PID_CalibEncoder(uint8_t drive_num, float value){
    if (osSemaphoreAcquire(DRIVE.semaphore, 100) == osOK){
        (*(DRIVE.encoder_s.v)) = GetEncoderFromRadial(value);
        DRIVE.position_l.sp.v = value;
        osSemaphoreRelease(DRIVE.semaphore);
    }
    return GetRealRadial((int32_t)(*(DRIVE.encoder_s.v)) - 0x7FFF);
}

void PID_StopDrive(uint8_t drive_num){
    if (drive_num > 3){
        if (osSemaphoreAcquire(drives[0].semaphore, 100) == osOK){
            drives[0].position_l.sp.v = drives[0].position_l.fb.v;
            osSemaphoreRelease(drives[0].semaphore);
        }
        if (osSemaphoreAcquire(drives[1].semaphore, 100) == osOK){
            drives[1].position_l.sp.v = drives[1].position_l.fb.v;
            osSemaphoreRelease(drives[1].semaphore);
        }
        if (osSemaphoreAcquire(drives[2].semaphore, 100) == osOK){
            drives[2].position_l.sp.v = drives[2].position_l.fb.v;
            osSemaphoreRelease(drives[2].semaphore);
        }
        if (osSemaphoreAcquire(drives[3].semaphore, 100) == osOK){
            drives[3].position_l.sp.v = drives[3].position_l.fb.v;
            osSemaphoreRelease(drives[3].semaphore);
        }
    }
    else{
        if (osSemaphoreAcquire(DRIVE.semaphore, 100) == osOK){
            DRIVE.position_l.sp.v = DRIVE.position_l.fb.v;
            osSemaphoreRelease(DRIVE.semaphore);
        }
    }
}

void PID_GetPositions(uint32_t * pos){
    if (osSemaphoreAcquire(drives[0].semaphore, 100) == osOK){
        pos[0] = *(drives[0].encoder_s.v);
        osSemaphoreRelease(drives[0].semaphore);
    }
    if (osSemaphoreAcquire(drives[1].semaphore, 100) == osOK){
        pos[1] = *(drives[1].encoder_s.v);
        osSemaphoreRelease(drives[1].semaphore);
    }
    if (osSemaphoreAcquire(drives[2].semaphore, 100) == osOK){
        pos[2] = *(drives[2].encoder_s.v);
        osSemaphoreRelease(drives[2].semaphore);
    }
    if (osSemaphoreAcquire(drives[3].semaphore, 100) == osOK){
        pos[3] = *(drives[3].encoder_s.v);
        osSemaphoreRelease(drives[3].semaphore);
    }
}

void PID_Task(void){
    
    drives[0].semaphore = osSemaphoreNew(1, 1, NULL);
    drives[1].semaphore = osSemaphoreNew(1, 1, NULL);
    drives[2].semaphore = osSemaphoreNew(1, 1, NULL);
    drives[3].semaphore = osSemaphoreNew(1, 1, NULL);
    
    PID_MoveSetpoints();

    _TIME_START_(led_notification, 100);
    
    drives[0].last_pid_call = osKernelGetTickCount();
    drives[1].last_pid_call = osKernelGetTickCount();
    drives[2].last_pid_call = osKernelGetTickCount();
    drives[3].last_pid_call = osKernelGetTickCount();
    osDelay(10);

    uint8_t emergency_stop_pressed = 0;

    while(1){

        if (_IS_EM_STOP_()){
            if (!emergency_stop_pressed){
                LED_PROTOCOL_UP();
                PID_StopDrive(4);
                emergency_stop_pressed = 1;
            }
            osDelay(10);
            continue;
        }
        else{
            emergency_stop_pressed = 0;
        }

        PID_DriveCompute(0);
        PID_DriveCompute(1);
        PID_DriveCompute(2);
        PID_DriveCompute(3);

        osDelay(10);

        if (_IS_TIMEOUT_(led_notification)){
            LED_PROTOCOL_TOGGLE();
            _TIME_UPDATE_(led_notification);
        }
    }
}