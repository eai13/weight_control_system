#ifndef _PID_CONFIG_H_
#define _PID_CONFIG_H_

// PID COEFS
#define WC_CONFIG_DEFAULT_CURRENT_Kp        ((float)(1))
#define WC_CONFIG_DEFAULT_CURRENT_Ki        ((float)(0))
#define WC_CONFIG_DEFAULT_CURRENT_Kd        ((float)(0))
#define WC_CONFIG_DEFAULT_SPEED_Kp          ((float)(30.0))
#define WC_CONFIG_DEFAULT_SPEED_Ki          ((float)(5))
#define WC_CONFIG_DEFAULT_SPEED_Kd          ((float)(6.0))
#define WC_CONFIG_DEFAULT_POSITION_Kp       ((float)(20.0))
#define WC_CONFIG_DEFAULT_POSITION_Ki       ((float)(0))
#define WC_CONFIG_DEFAULT_POSITION_Kd       ((float)(0))

// ACCUMULATOR THRESHOLDS
#define WC_CONFIG_DEFAULT_CURR_ACC_THRES    ((float)(2.0))
#define WC_CONFIG_DEFAULT_SPEED_ACC_THRES   ((float)(100))
#define WC_CONFIG_DEFAULT_POS_ACC_THRES     ((float)(0))

// #define WC_CONFIG_DEFAULT_OUTPUT_THRES      (PWM_UPPER_COUNT / 2)
// ((uint32_t)(16000))

// TIME CONSTANT FOR INTEGRATOR
#define TIME_CONSTANT   ((float)(0.01))

#endif