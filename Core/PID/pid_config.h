#ifndef _PID_CONFIG_H_
#define _PID_CONFIG_H_

// PID COEFS
#define CURRENT_Kp      ((float)(0.35))
#define CURRENT_Ki      ((float)(0.50))
#define SPEED_Kp        ((float)(30.0))
#define POSITION_Kp     ((float)(20.0))

// ACCUMULATOR THRESHOLDS
#define ACC_LIM_UP      ((float)(2.0))
#define ACC_LIM_DOWN    ((float)(-2.0))

// TIME CONSTANT FOR INTEGRATOR
#define TIME_CONSTANT   ((float)(0.1))

#endif