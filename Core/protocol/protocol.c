#include "protocol.h"
#include "global_config.h"
#include "pid.h"

#include "stm32g071xx.h"
#include "usart.h"
#include "main.h"

#define MAKE_BP_HEADER(CMD, W_SIZE) \
    p_bp->cmd = CMD; \
    p_bp->w_size = W_SIZE;

#define MAKE_CONTROL_HEADER(ID, CMD) \
    p_cnt->id = ID; \
    p_cnt->cmd = CMD;

static volatile protocol_header_e HeaderToReceive = HEADER_BASIC;

static volatile uint8_t Protocol_ProcessFlag = 0;

uint8_t PROTOCOL_MessagePending(void){
    return Protocol_ProcessFlag;
}

void PROTOCOL_ResetPendingFlag(void){
    Protocol_ProcessFlag = 0;
}

static volatile uint8_t                 tx_buffer[150];
static volatile bp_header_t *           p_bp = tx_buffer;
static volatile control_header_t *      p_cnt;
static volatile single_reg_data_t *     p_s_data;
static volatile multiple_reg_data_t *   p_m_data;
static volatile global_cmd_data_t *     p_c_data;
static volatile wc_plottables_t *       p_plottable[4];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &PROTOCOL_UART){
        switch(HeaderToReceive){
            case(HEADER_BASIC):
                if (p_bp->w_size){
                    HAL_UART_Receive_IT(&PROTOCOL_UART, tx_buffer + sizeof(bp_header_t), p_bp->w_size * 4);
                    HeaderToReceive = HEADER_DATA;
                }
                else{
                    Protocol_ProcessFlag = 1;
                }
                break;
            case(HEADER_DATA):
                HeaderToReceive = HEADER_BASIC;
                Protocol_ProcessFlag = 1;
                break;    
        }
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &PROTOCOL_UART){
        HAL_UART_Receive_IT(&PROTOCOL_UART, (uint8_t *)p_bp, sizeof(bp_header_t));
    }
}

void PROTOCOL_ProcessFrame(void){
    switch(p_bp->cmd){
        case(BP_CMD_PING):{
            MAKE_BP_HEADER(BP_CMD_PING, 1);
            tx_buffer[5] = 'A'; tx_buffer[6] = 'P'; tx_buffer[7] = 'P'; tx_buffer[8] = '1';
            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + 4);
            break;
        }
        case(BP_CMD_JUMP):{
            if (*((uint32_t *)(tx_buffer + sizeof(bp_header_t))) != 0){
                MAKE_BP_HEADER(BP_ERROR_UNKNOWN_CMD, 0);
                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t));
                break;
            }
            MAKE_BP_HEADER(BP_CMD_JUMP, 0);
            HAL_UART_Transmit(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t), 10);
            MEMORY_SetActualPosition(ENCODER_1_COUNT, ENCODER_2_COUNT, ENCODER_3_COUNT, PID_GetCorrector() + ENCODER_4_COUNT, 10);
            HAL_NVIC_SystemReset();
        }
        case(BP_CMD_CONTROL):{
            p_cnt = (control_header_t *)(tx_buffer + sizeof(bp_header_t));
            if (p_cnt->id >= ID_LAST){
                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                break;
            }
            switch(p_cnt->id){
                case(ID_GLOBAL):
                    switch(p_cnt->cmd){
                        case(CMD_READ_PLOTTABLE):{
                            for (uint8_t iter = 0; iter < 4; iter++){
                                p_plottable[iter] = (wc_plottables_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(wc_plottables_t) * iter);
                                *(p_plottable[iter]) = PID_ReadPlottables(iter);
                            }
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 30);
                            MAKE_CONTROL_HEADER(p_cnt->id, p_cnt->cmd);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(wc_plottables_t) * 4);
                            break;
                        }
                        case(CMD_WRITE_REG):{
                            p_m_data = (multiple_reg_data_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t));
                            if (p_m_data->reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                break;
                            }
                            for (uint8_t iter = 0; iter < 4; iter++){
                                if (PID_WriteReg(iter, p_m_data->reg, p_m_data->data[iter]) == RW_ERROR){
                                    MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                    MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                    HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                    break;
                                }
                            }
                            if (p_cnt->cmd == CMD_ERROR) break;
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 7);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(multiple_reg_data_t));
                            break;
                        }
                        case(CMD_READ_REG):{
                            p_m_data = (multiple_reg_data_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t));
                            if (p_m_data->reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                break;
                            }
                            float tmp = 0;
                            for (uint8_t iter = 0; iter < 4; iter++){
                                if (PID_ReadReg(iter, p_m_data->reg, &tmp) == RW_ERROR){
                                    MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                    MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                    HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                    break;
                                }
                                else p_m_data->data[iter] = tmp;
                            }
                            if (p_cnt->cmd == CMD_ERROR) break;
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 7);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(multiple_reg_data_t));
                            break;
                        }
                        case(CMD_CALIBRATE_AS_ZERO_POSITION):{
                            PID_SetZero(4);
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                            break;
                        }
                        case(CMD_STOP_DRIVE):{
                            PID_StopDrive(4);
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                            break;
                        }
                        default:{
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                            MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                            break;
                        }
                    }
                    break;
                case(ID_GLOBAL_CMD):
                    switch (p_cnt->cmd){
                        case(CMD_BLOCK_DRIVE):{
                            // +
                            break;
                        }
                        case(CMD_ENABLE_DRIVE):{
                            // +
                            break;
                        }
                        case(CMD_RESET_SYSTEM):{
                            // +
                            break;
                        }
                        case(CMD_EXIT_APP):{
                            // +
                            break;
                        }
                        default:{
                            MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                            break;
                        }
                    }
                    MAKE_BP_HEADER(p_bp->cmd, 2);
                    HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                    break;
                default:
                    switch(p_cnt->cmd){
                        case(CMD_WRITE_REG):{
                            p_s_data = (single_reg_data_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t));
                            if (p_s_data->reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                break;
                            }
                            if (PID_WriteReg(p_cnt->id, p_s_data->reg, p_s_data->data) == RW_ERROR){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                break;
                            }
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 4);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(single_reg_data_t));
                            break;
                        }
                        case(CMD_READ_REG):{
                            p_s_data = tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t);
                            if (p_s_data->reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                break;
                            }
                            float tmp = 0;
                            if (PID_ReadReg(p_cnt->id, p_s_data->reg, &tmp) == RW_ERROR){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                break;
                            }
                            else p_s_data->data = tmp;
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 4);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(single_reg_data_t));
                            break;
                        }
                        case(CMD_CALIBRATE_AS_ZERO_POSITION):{
                            PID_SetZero(p_cnt->id);
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                            break;
                        }
                        case(CMD_STOP_DRIVE):{
                            PID_StopDrive(p_cnt->id);
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                            break;
                        }
                        default:{
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                            MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                            break;
                        }
                    }
                    break;
            }
            break;
        }
        default:
            MAKE_BP_HEADER(BP_ERROR_UNKNOWN_CMD, 0);
            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t));
            break;
    }
}

void PROTOCOL_Start(void){
    HAL_UART_Receive_IT(&PROTOCOL_UART, (uint8_t *)p_bp, sizeof(bp_header_t));
}