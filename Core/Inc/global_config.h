#ifndef _GLOBAL_CONFIG_H_
#define _GLOBAL_CONFIG_H_

#define _DEBUG_VERSION_

// DEBUG USART
#define DEBUG_UART      huart2

// PROTOCOL USART
#define PROTOCOL_UART   huart1

// ENCODER TIMERS
#define ENCODER_1_TIMER     htim2
#define ENCODER_1_COUNT     (TIM2->CNT)
#define ENCODER_2_TIMER     htim1
#define ENCODER_2_COUNT     (TIM1->CNT)
#define ENCODER_3_TIMER     htim3
#define ENCODER_3_COUNT     (TIM3->CNT)
#define ENCODER_4_TIMER     hlptim1
#define ENCODER_4_COUNT     (LPTIM1->CNT)

// PWM TIMERS
#define PWM_1_TIMER         htim15
#define PWM_1_CHANNEL       TIM_CHANNEL_1
#define PWM_1_DUTY          (TIM15->CCR1)
#define PWM_2_TIMER         htim15
#define PWM_2_CHANNEL       TIM_CHANNEL_2
#define PWM_2_DUTY          (TIM15->CCR2)
#define PWM_3_TIMER         htim16
#define PWM_3_CHANNEL       TIM_CHANNEL_1
#define PWM_3_DUTY          (TIM16->CCR1)
#define PWM_4_TIMER         htim17
#define PWM_4_CHANNEL       TIM_CHANNEL_1
#define PWM_4_DUTY          (TIM17->CCR1)

// DRIVE COMPUTE TIMER
#define CONTROL_SYSTEM_TIMER      htim14

#define PWM_UPPER_COUNT     32000

#endif