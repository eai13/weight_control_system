#ifndef _GLOBAL_CONFIG_H_
#define _GLOBAL_CONFIG_H_

#define _DEBUG_VERSION_

// DEBUG USART
#define DEBUG_UART      huart5

// PROTOCOL USART
#define PROTOCOL_UART   huart4

// ENCODER TIMERS
#define ENCODER_1_TIMER     htim1
#define ENCODER_1_COUNT     (TIM1->CNT)
#define ENCODER_2_TIMER     htim2
#define ENCODER_2_COUNT     (TIM2->CNT)
#define ENCODER_3_TIMER     htim3
#define ENCODER_3_COUNT     (TIM3->CNT)
#define ENCODER_4_TIMER     htim4
#define ENCODER_4_COUNT     (TIM4->CNT)

// PWM TIMERS
#define PWM_1_TIMER         htim9
#define PWM_1_CHANNEL       TIM_CHANNEL_1
#define PWM_1_DUTY          (TIM9->CCR1)
#define PWM_2_TIMER         htim9
#define PWM_2_CHANNEL       TIM_CHANNEL_2
#define PWM_2_DUTY          (TIM9->CCR2)
#define PWM_3_TIMER         htim10
#define PWM_3_CHANNEL       TIM_CHANNEL_1
#define PWM_3_DUTY          (TIM10->CCR1)
#define PWM_4_TIMER         htim11
#define PWM_4_CHANNEL       TIM_CHANNEL_1
#define PWM_4_DUTY          (TIM11->CCR1)

#define PWM_UPPER_COUNT     2000

#endif