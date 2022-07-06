#ifndef _PID_CONFIG_H_
#define _PID_CONFIG_H_

// PID COEFS
#define CURRENT_Kp      ((float)(0.35))
#define CURRENT_Ki      ((float)(0))
#define SPEED_Kp        ((float)(30.0))
#define SPEED_Ki        ((float)(5))
#define POSITION_Kp     ((float)(20.0))
#define POSITION_Ki     ((float)(0))

// ACCUMULATOR THRESHOLDS
#define CURR_ACC_LIM_UP         ((float)(2.0))
#define CURR_ACC_LIM_DOWN       ((float)(-2.0))
#define SPEED_ACC_LIM_UP        ((float)(100))
#define SPEED_ACC_LIM_DOWN      ((float)(-100))
#define POSITION_ACC_LIM_UP     ((float)(0))
#define POSITION_ACC_LIM_DOWN   ((float)(0))

// TIME CONSTANT FOR INTEGRATOR
#define TIME_CONSTANT   ((float)(0.1))

#endif