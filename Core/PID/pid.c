#include "pid.h"
#include "pid_config.h"
#include "global_config.h"
#include "drive_config.h"
#include "stm32g071xx.h"
#include "main.h"
#include <math.h>
#include <stdint.h>

#include "lptim.h"

#ifdef _DEBUG_VERSION_
#include "debug.h"
#endif

#define DRIVE   (drives[drive_num])
#define _ABS_FLOAT(value) \
            ((((float)(value)) > ((float)(0.0))) ? ((float)(value)) : (((float)(-1)) * ((float)(value))))

uint16_t current[4] = { 0, 0, 0, 0 };
float    set_position[4] = { 0, 0, 0, 0 };
int32_t lptim_corrector = 0;

void HAL_LPTIM_AutoReloadMatchCallback(LPTIM_HandleTypeDef *hlptim){
  if (hlptim == &ENCODER_4_TIMER){
    if (ENCODER_4_COUNT < 0x7FFF)
      lptim_corrector += 0xFFFF;
    else
      lptim_corrector -= 0xFFFF;
  }
}

device_t drives[4] = {
    { // DRIVE 1
        .torque = { RW, 0 },
        .position_l = { 
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, POSITION_Kp },
            .Ki =   { RO, 0 },
            .Kd =   { RO, 0 }
        },
        .speed_l = {
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, SPEED_Kp},
            .Ki =   { RO, 0 },
            .Kd =   { RO, 0 }
        },
        .current_l = {
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, CURRENT_Kp },
            .Ki =   { RW, CURRENT_Ki },
            .Kd =   { RO, 0 }
        },
        .output =       { RO, 0 },
        .encoder_s =    { RW, &ENCODER_1_COUNT },
        .current_s =    { RO, &(current[0]) },
        .pwm_duty =     { RO, &PWM_1_DUTY },
        .dir_pins =     {
            .frw_port = DRV1_DIR_0_GPIO_Port,
            .frw_pin =  DRV1_DIR_0_Pin,
            .rev_port = DRV1_DIR_1_GPIO_Port,
            .rev_pin =  DRV1_DIR_1_Pin
        }
    },
    { // DRIVE 2
        .torque = { RW, 0 },
        .position_l = { 
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, POSITION_Kp },
            .Ki =   { RO, 0 },
            .Kd =   { RO, 0 }
        },
        .speed_l = {
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, SPEED_Kp},
            .Ki =   { RO, 0 },
            .Kd =   { RO, 0 }
        },
        .current_l = {
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, CURRENT_Kp },
            .Ki =   { RW, CURRENT_Ki },
            .Kd =   { RO, 0 }
        },
        .output =       { RO, 0 },
        .encoder_s =    { RW, &ENCODER_2_COUNT },
        .current_s =    { RO, &(current[1]) },
        .pwm_duty =     { RO, &PWM_2_DUTY },
        .dir_pins =     {
            .frw_port = DRV2_DIR_0_GPIO_Port,
            .frw_pin =  DRV2_DIR_0_Pin,
            .rev_port = DRV2_DIR_1_GPIO_Port,
            .rev_pin =  DRV2_DIR_1_Pin
        }
    },
    { // DRIVE 3
        .torque = { RW, 0 },
        .position_l = { 
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, POSITION_Kp },
            .Ki =   { RO, 0 },
            .Kd =   { RO, 0 }
        },
        .speed_l = {
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, SPEED_Kp},
            .Ki =   { RO, 0 },
            .Kd =   { RO, 0 }
        },
        .current_l = {
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, CURRENT_Kp },
            .Ki =   { RW, CURRENT_Ki },
            .Kd =   { RO, 0 }
        },
        .output =       { RO, 0 },
        .encoder_s =    { RW, &ENCODER_3_COUNT },
        .current_s =    { RO, &(current[2]) },
        .pwm_duty =     { RO, &PWM_3_DUTY },
        .dir_pins =     {
            .frw_port = DRV3_DIR_0_GPIO_Port,
            .frw_pin =  DRV3_DIR_0_Pin,
            .rev_port = DRV3_DIR_1_GPIO_Port,
            .rev_pin =  DRV3_DIR_1_Pin
        }
    },
    { // DRIVE 4
        .torque = { RW, 0 },
        .position_l = { 
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, POSITION_Kp },
            .Ki =   { RO, 0 },
            .Kd =   { RO, 0 }
        },
        .speed_l = {
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, SPEED_Kp},
            .Ki =   { RO, 0 },
            .Kd =   { RO, 0 }
        },
        .current_l = {
            .sp =   { RW, 0 },
            .fb =   { RO, 0 },
            .acc =  { RO, 0 },
            .Kp =   { RW, CURRENT_Kp },
            .Ki =   { RW, CURRENT_Ki },
            .Kd =   { RO, 0 }
        },
        .output =       { RO, 0 },
        .encoder_s =    { RW, &ENCODER_4_COUNT },
        .current_s =    { RO, &(current[3]) },
        .pwm_duty =     { RO, &PWM_4_DUTY },
        .dir_pins =     {
            .frw_port = DRV4_DIR_0_GPIO_Port,
            .frw_pin =  DRV4_DIR_0_Pin,
            .rev_port = DRV4_DIR_1_GPIO_Port,
            .rev_pin =  DRV4_DIR_1_Pin
        }
    }
};

static inline float GetRealCurrent(uint16_t raw){
    // ((double)(abs(((int32_t)(bits)) - AVG_CURRENT_MEASURE)) / ADC_RESOLUTION * REF_VOLTAGE / VOLTS_PER_AMP)
    return (((float)(abs((int16_t)(raw)) - ADC_CURRENT_0)) / ADC_RESOLUTION * ADC_REFERENCE / ADC_VPA);
}

static inline float GetRealRadial(uint32_t raw){
    return (((float)(raw)) / ((float)(600.0)) * ((float)(6.28)) / ((float)(2.0)));
}

static inline uint32_t VoltsToPWM(float volts){
    return ((int32_t)((volts) / ((float)(24.0)) * ((float)(64000.0))));
}

static inline void DriveForward(uint8_t drive_num){
    HAL_GPIO_WritePin(DRIVE.dir_pins.frw_port, DRIVE.dir_pins.frw_pin, 1);
    HAL_GPIO_WritePin(DRIVE.dir_pins.rev_port, DRIVE.dir_pins.rev_pin, 0);
}

static inline void DriveReverse(uint8_t drive_num){
    HAL_GPIO_WritePin(DRIVE.dir_pins.rev_port, DRIVE.dir_pins.rev_pin, 1);
    HAL_GPIO_WritePin(DRIVE.dir_pins.frw_port, DRIVE.dir_pins.frw_pin, 0);
}

void PID_DriveCompute(uint8_t drive_num){
    print_in("Current values: %d, %d, %d, %d\r\n", current[0], current[1], current[2], current[3]);
    print_in("Encoder values: %d, %d, %d, %d\r\n",
            *(drives[0].encoder_s.v), *(drives[1].encoder_s.v), *(drives[2].encoder_s.v), *(drives[3].encoder_s.v));
    
    // // GETTING RADIAL POSITION
    float tmp_position = 0;
    if (drive_num == 3){
        tmp_position = GetRealRadial(((int32_t)(*(DRIVE.encoder_s.v))) + lptim_corrector);
    }
    else if (drive_num = 0){
        tmp_position = GetRealRadial(((int32_t)(*(DRIVE.encoder_s.v))) - 0x7FFFFFFF);
    }
    else{
        tmp_position = GetRealRadial(((int32_t)(*(DRIVE.encoder_s.v))) - 0x7FFF);
    }

    // // FEEDBACK COMPUTE
    // DRIVE.position_l.fb.v   =
    //         tmp_position;
    // DRIVE.speed_l.fb.v      = 
    //         (tmp_position - DRIVE.position_l.fb.v) / TIME_CONSTANT;
    // DRIVE.current_l.fb.v    =
    //         GetRealCurrent(*(DRIVE.current_s.v));

    // // SETPOINT COMPUTE
    // DRIVE.position_l.sp.v = 
    //         set_position[drive_num];
    // DRIVE.speed_l.sp.v = 
    //         (DRIVE.position_l.sp.v - DRIVE.position_l.fb.v) * DRIVE.position_l.Kp.v;
    // DRIVE.current_l.sp.v = 
    //         (DRIVE.speed_l.sp.v - DRIVE.speed_l.fb.v) * DRIVE.speed_l.Kp.v;

    // // CURRENT ACCUMULATOR COMPUTE
    // DRIVE.current_l.acc.v += 
    //         (DRIVE.current_l.sp.v / DRIVE_REDUCTION + DRIVE.torque.v - DRIVE.current_l.fb.v) * DRIVE.current_l.Ki.v;
    // if (DRIVE.current_l.acc.v > ACC_LIM_UP)
    //     DRIVE.current_l.acc.v = ACC_LIM_UP;
    // else if (DRIVE.current_l.acc.v < ACC_LIM_DOWN)
    //     DRIVE.current_l.acc.v = ACC_LIM_DOWN;

    // // OUTPUT COMPUTE
    // DRIVE.output.v = 
    //         (DRIVE.current_l.sp.v / DRIVE_REDUCTION + DRIVE.torque.v - DRIVE.current_l.fb.v) * DRIVE.current_l.Kp.v + DRIVE.current_l.acc.v;
    // if (DRIVE.output.v > ((float)(24.0))) DRIVE.output.v = ((float)(24.0));
    // else if (DRIVE.output.v < ((float)(-24.0))) DRIVE.output.v = ((float)(-24.0));

    // // SET DIRECTION
    // if (DRIVE.output.v > 0) DriveForward(drive_num);
    // else DriveReverse(drive_num);

    // // CONVERT TO PWM
    // uint16_t pwm_out = VoltsToPWM(_ABS_FLOAT(DRIVE.output.v));
    // if (pwm_out > 32000)
    //     *(DRIVE.pwm_duty.v) = 32000;
    // else
    //     *(DRIVE.pwm_duty.v) = pwm_out;
}