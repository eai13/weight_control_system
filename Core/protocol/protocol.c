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
// static volatile uint8_t                 buffer[128];
// static volatile bp_header_t             bp_header;
static volatile bp_header_t *           p_bp = tx_buffer;
static volatile control_header_t *      p_cnt;
static volatile single_reg_data_t *     p_s_data;
static volatile multiple_reg_data_t *   p_m_data;
static volatile global_cmd_data_t *     p_c_data;
static volatile wc_plottables_t *       p_plottable[4];

// static volatile control_header_t        cnt_header;
// static volatile single_reg_data_t       s_data;
// static volatile multiple_reg_data_t     m_data;
// static volatile global_cmd_data_t       c_data;
// static volatile response_header_t       cnt_resp_header;

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
        // HAL_UART_Transmit(&huart2, "TxCallback\r\n", 12, 10);
        HAL_UART_Receive_IT(&PROTOCOL_UART, (uint8_t *)p_bp, sizeof(bp_header_t));
    }
}

void PROTOCOL_ProcessFrame(void){
    switch(p_bp->cmd){
        case(BP_CMD_PING):{
            MAKE_BP_HEADER(BP_CMD_PING, 1);
            tx_buffer[5] = 'A'; tx_buffer[6] = 'P'; tx_buffer[7] = 'P'; tx_buffer[8] = '1';
            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + 4);
            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
            // HAL_UART_Transmit(&PROTOCOL_UART, SELF_ID, 4, 10);
            break;
        }
        case(BP_CMD_JUMP):{
            if (*((uint32_t *)(tx_buffer + sizeof(bp_header_t))) != 0){
                MAKE_BP_HEADER(BP_ERROR_UNKNOWN_CMD, 0);
                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t));
                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                break;
            }
            MAKE_BP_HEADER(BP_CMD_JUMP, 0);
            HAL_UART_Transmit(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t), 10);
            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);

            HAL_NVIC_SystemReset();
        }
        case(BP_CMD_CONTROL):
            p_cnt = (control_header_t *)(tx_buffer + sizeof(bp_header_t));
            // memcpy((void *)&cnt_header, buffer, sizeof(control_header_t));
            if (p_cnt->id >= ID_LAST){
                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                break;
            }
            switch(p_cnt->id){
                case(ID_GLOBAL):
                    switch(p_cnt->cmd){
                        case(CMD_READ_PLOTTABLE):{
                            // HAL_UART_Transmit(&huart2, "Plottables\r\n", 12, 10);
                            for (uint8_t iter = 0; iter < 4; iter++){
                                p_plottable[iter] = (wc_plottables_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(wc_plottables_t) * iter);
                                *(p_plottable[iter]) = PID_ReadPlottables(iter);
                                // PID_ReadReg(iter, REGISTER_POS_SP, &tmp);
                                // *((float *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t)) + 0 + iter * 7) = tmp;
                                // PID_ReadReg(iter, REGISTER_POS_FB, &tmp);
                                // *((float *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t)) + 1 + iter * 7) = tmp;
                                // PID_ReadReg(iter, REGISTER_SPD_SP, &tmp);
                                // *((float *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t)) + 2 + iter * 7) = tmp;
                                // PID_ReadReg(iter, REGISTER_SPD_FB, &tmp);
                                // *((float *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t)) + 3 + iter * 7) = tmp;
                                // PID_ReadReg(iter, REGISTER_CUR_SP, &tmp);
                                // *((float *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t)) + 4 + iter * 7) = tmp;
                                // PID_ReadReg(iter, REGISTER_CUR_FB, &tmp);
                                // *((float *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t)) + 5 + iter * 7) = tmp;
                                // PID_ReadReg(iter, REGISTER_OUTPUT, &tmp);
                                // *((float *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t)) + 6 + iter * 7) = tmp;
                            }
                            // HAL_UART_Transmit(&huart2, "Fuuuh\r\n", 7, 10);
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 30);
                            MAKE_CONTROL_HEADER(p_cnt->id, p_cnt->cmd);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(wc_plottables_t) * 4);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(cnt_header), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)plottable_data, 112, 10);
                            break;
                        }
                        case(CMD_WRITE_REG):{
                            p_m_data = (multiple_reg_data_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t));
                            // memcpy((void *)&m_data, buffer + sizeof(control_header_t), sizeof(multiple_reg_data_t));
                            if (p_m_data->reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            for (uint8_t iter = 0; iter < 4; iter++){
                                if (PID_WriteReg(iter, p_m_data->reg, p_m_data->data[iter]) == RW_ERROR){
                                    MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                    MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                    HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                    // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                    // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                    break;
                                }
                            }
                            if (p_cnt->cmd == CMD_ERROR) break;
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 7);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(multiple_reg_data_t));
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&m_data, sizeof(multiple_reg_data_t), 10);
                            break;
                        }
                        case(CMD_READ_REG):{
                            p_m_data = (multiple_reg_data_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t));
                            // memcpy((void *)&m_data, buffer + sizeof(control_header_t), sizeof(multiple_reg_data_t));
                            if (p_m_data->reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            float tmp = 0;
                            for (uint8_t iter = 0; iter < 4; iter++){
                                if (PID_ReadReg(iter, p_m_data->reg, &tmp) == RW_ERROR){
                                    MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                    MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                    HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                    // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                    // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                    break;
                                }
                                else p_m_data->data[iter] = tmp;
                            }
                            if (p_cnt->cmd == CMD_ERROR) break;
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 7);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(multiple_reg_data_t));
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&m_data, sizeof(multiple_reg_data_t), 10);
                            break;
                        }
                        default:{
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                            MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            break;
                        }
                    }
                    break;
                case(ID_GLOBAL_CMD):
                    switch (p_cnt->cmd){
                        case(CMD_BLOCK_DRIVE):
                            // +
                            break;
                        case(CMD_ENABLE_DRIVE):
                            // +
                            break;
                        case(CMD_RESET_SYSTEM):
                            // +
                            break;
                        case(CMD_EXIT_APP):
                            // +
                            break;
                        default:
                            MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                            break;
                    }
                    MAKE_BP_HEADER(p_bp->cmd, 2);
                    HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                    // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                    // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                    break;
                default:
                    switch(p_cnt->cmd){
                        case(CMD_WRITE_REG):
                            p_s_data = (single_reg_data_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t));
                            // memcpy((void *)&s_data, buffer + sizeof(control_header_t), sizeof(single_reg_data_t));
                            if (p_s_data->reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            if (PID_WriteReg(p_cnt->id, p_s_data->reg, p_s_data->data) == RW_ERROR){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 4);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(single_reg_data_t));
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&s_data, sizeof(single_reg_data_t), 10);
                            break;
                        case(CMD_READ_REG): ///////// HEHRHEHHERHERHERHERHERUERHREHUERUHERUHERUHERUHREHUREHUREHUREHUERUHREUHREUE
                            p_s_data = tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t);
                            // memcpy((void *)&s_data, buffer + sizeof(control_header_t), sizeof(single_reg_data_t));
                            if (p_s_data->reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            float tmp = 0;
                            if (PID_ReadReg(p_cnt->id, p_s_data->reg, &tmp) == RW_ERROR){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                                HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            else p_s_data->data = tmp;
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 4);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(single_reg_data_t));
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&s_data, sizeof(single_reg_data_t), 10);
                            break;
                        default:
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                            MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
                            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            break;
                    }
                    break;
            }
            break;
        default:
            MAKE_BP_HEADER(BP_ERROR_UNKNOWN_CMD, 0);
            HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t));
            // HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
            break;
    }
    // HAL_UART_Receive_IT(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t));

    // if (header.id >= ID_LAST){
    //     resp_header.id = header.id;
    //     resp_header.status = ERROR_ID;
    //     HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
    //     HAL_UART_Receive_IT(&PROTOCOL_UART, &header, sizeof(control_header_t));
    //     return;
    // }
    // resp_header.id = header.id;
    // switch(header.id){
    //     case(ID_GLOBAL):
    //         switch (header.cmd){
    //             case (CMD_WRITE_REG):
    //                 if (HAL_UART_Receive(&PROTOCOL_UART, &m_data, sizeof(multiple_reg_data_t), 10) != HAL_OK){
    //                     break;
    //                 }
    //                 // Check CRC
    //                 resp_header.status = ERROR_NO_ERROR;
    //                 for (uint8_t iter = 0; iter < 4; iter++){
    //                     if (PID_WriteReg(iter, m_data.reg, m_data.data[iter]) == RW_ERROR){
    //                         resp_header.status = ERROR_WRITE;
    //                     }
    //                 }
    //                 HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
    //                 break;
    //             case (CMD_READ_REG):
    //                 if (HAL_UART_Receive(&PROTOCOL_UART, &m_data, sizeof(multiple_reg_data_t), 10) != HAL_OK)
    //                 {
    //                     break;
    //                 }
    //                 // Check CRC
    //                 resp_header.status = ERROR_NO_ERROR;
    //                 float tmp = 0;
    //                 for (uint8_t iter = 0; iter < 4; iter++){
    //                     if (PID_ReadReg(iter, m_data.reg, &tmp) == RW_ERROR){
    //                         resp_header.status = ERROR_READ;
    //                     }
    //                     else m_data.data[iter] = tmp;
    //                 }
    //                 // Count CRC
    //                 HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
    //                 HAL_UART_Transmit(&PROTOCOL_UART, &m_data, sizeof(multiple_reg_data_t), 10);
    //                 break;
    //             default:
    //                 resp_header.status = ERROR_CMD;
    //                 HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
    //                 break;
    //         }
    //         break;
    //     case(ID_GLOBAL_CMD):
    //         switch (header.cmd){
    //             case(CMD_BLOCK_DRIVE):
    //                 // +
    //                 resp_header.status = ERROR_NO_ERROR;
    //                 break;
    //             case(CMD_ENABLE_DRIVE):
    //                 // +
    //                 resp_header.status = ERROR_NO_ERROR;
    //                 break;
    //             case(CMD_RESET_SYSTEM):
    //                 // +
    //                 resp_header.status = ERROR_NO_ERROR;
    //                 break;
    //             case(CMD_EXIT_APP):
    //                 // +
    //                 resp_header.status = ERROR_NO_ERROR;
    //                 break;
    //             default:
    //                 resp_header.status = ERROR_CMD;
    //                 break;
    //         }
    //         HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
    //         break;
    //     default:
    //         switch (header.cmd){
    //             case(CMD_READ_REG):
    //                 if (HAL_UART_Receive(&PROTOCOL_UART, &s_data, sizeof(single_reg_data_t), 10) != HAL_OK){
    //                     break;
    //                 }
    //                 // Check Crc
    //                 float tmp = 0;
    //                 if (PID_ReadReg(header.id, s_data.reg, &tmp) == RW_OK){
    //                     resp_header.status = ERROR_NO_ERROR;
    //                     s_data.data = tmp;
    //                 }
    //                 else resp_header.status = ERROR_READ;
    //                 // Count Crc
    //                 HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
    //                 HAL_UART_Transmit(&PROTOCOL_UART, &s_data, sizeof(single_reg_data_t), 10);
    //                 break;
    //             case(CMD_WRITE_REG):
    //                 if (HAL_UART_Receive(&PROTOCOL_UART, &s_data, sizeof(single_reg_data_t), 10) != HAL_OK){
    //                     break;
    //                 }
    //                 // Check Crc
    //                 if (PID_WriteReg(header.id, s_data.reg, s_data.data) == RW_OK)
    //                     resp_header.status = ERROR_NO_ERROR;
    //                 else resp_header.status = ERROR_WRITE;
    //                 HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
    //                 break;
    //             default:
    //                 resp_header.status = ERROR_CMD;
    //                 HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
    //                 break;
    //         }
    //         break;
    // }
    // HAL_UART_Receive_IT(&PROTOCOL_UART, &header, sizeof(control_header_t));
}

void PROTOCOL_Start(void){
    HAL_UART_Receive_IT(&PROTOCOL_UART, (uint8_t *)p_bp, sizeof(bp_header_t));
}