#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#define BOOTLOADER_ID   "BOOT"
#define BOOTLOADER_TAB  0

#define APP_1_ID        "APP1"
#define APP_1_TAB       1

#define APP_2_ID        "APP2"
#define APP_2_TAB       2

#define USER_ID         "USER"
#define USER_TAB        3

#define POLYGON_X_SIZE      100
#define POLYGON_Y_SIZE      100
#define POLYGON_Z_SIZE      100

#define M_PER_DOT           (float)(0.05)

#define DEFAULT_DIAL_RADIUS 10
#define ROPE_THICK          2
#define ROPE_LENGTH         2000

#define PI                  3.1415

#define MOTOR_1_A_CALIB     ((double)(-0.00028))
#define MOTOR_1_B_CALIB     ((double)(0.09556))
#define MOTOR_1_C_CALIB     ((double)(0.12712))
#define MOTOR_1_MIN         ((double)(0))
#define MOTOR_1_MAX         ((double)(50.24))

#define MOTOR_2_A_CALIB     ((double)(-0.00026))
#define MOTOR_2_B_CALIB     ((double)(0.09191))
#define MOTOR_2_C_CALIB     ((double)(0.12809))
#define MOTOR_2_MIN         ((double)(0))
#define MOTOR_2_MAX         ((double)(50.24))

#define MOTOR_3_A_CALIB     ((double)(-0.00028))
#define MOTOR_3_B_CALIB     ((double)(0.09638))
#define MOTOR_3_C_CALIB     ((double)(0.13003))
#define MOTOR_3_MIN         ((double)(0))
#define MOTOR_3_MAX         ((double)(50.24))

#define MOTOR_4_A_CALIB     ((double)(-0.00026))
#define MOTOR_4_B_CALIB     ((double)(0.09128))
#define MOTOR_4_C_CALIB     ((double)(0.12630))
#define MOTOR_4_MIN         ((double)(0))
#define MOTOR_4_MAX         ((double)(50.24))

#endif // GLOBAL_CONFIG_H
