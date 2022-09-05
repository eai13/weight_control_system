#include "protocol.h"
#include "global_config.h"
#include "pid.h"

#include "stm32g071xx.h"
#include "usart.h"

#define MAKE_BP_HEADER(CMD, W_SIZE) \
    bp_header.cmd = CMD; \
    bp_header.w_size = W_SIZE;

#define MAKE_CONTROL_HEADER(ID, CMD) \
    cnt_header.id = ID; \
    cnt_header.cmd = CMD;

static volatile protocol_header_e HeaderToReceive = HEADER_BASIC;

static volatile uint8_t Protocol_ProcessFlag = 0;

uint8_t PROTOCOL_MessagePending(void){
    return Protocol_ProcessFlag;
}

void PROTOCOL_ResetPendingFlag(void){
    Protocol_ProcessFlag = 0;
}

static volatile bp_header_t             bp_header;
static volatile uint8_t                 buffer[128];
static volatile control_header_t        cnt_header;
static volatile single_reg_data_t       s_data;
static volatile multiple_reg_data_t     m_data;
static volatile global_cmd_data_t       c_data;
static volatile response_header_t       cnt_resp_header;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &PROTOCOL_UART){
        switch(HeaderToReceive){
            case(HEADER_BASIC):
                if (bp_header.w_size){
                    HAL_UART_Receive_IT(&PROTOCOL_UART, buffer, bp_header.w_size * 4);
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

void PROTOCOL_ProcessFrame(void){
    switch(bp_header.cmd){
        case(BP_CMD_PING):
            MAKE_BP_HEADER(BP_CMD_PING, 1);
            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
            HAL_UART_Transmit(&PROTOCOL_UART, SELF_ID, 4, 10);
            break;
        case(BP_CMD_JUMP):
            if (*((uint32_t *)buffer) != 0){
                MAKE_BP_HEADER(BP_ERROR_UNKNOWN_CMD, 0);
                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                break;
            }
            MAKE_BP_HEADER(BP_CMD_JUMP, 0);
            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);

            HAL_NVIC_SystemReset();
            
        case(BP_CMD_CONTROL):
            memcpy((void *)&cnt_header, buffer, sizeof(control_header_t));
            if (cnt_header.id >= ID_LAST){
                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                break;
            }
            switch(cnt_header.id){
                case(ID_GLOBAL):
                    switch(cnt_header.cmd){
                        case(CMD_WRITE_REG):
                            memcpy((void *)&m_data, buffer + sizeof(control_header_t), sizeof(multiple_reg_data_t));
                            if (m_data.reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            for (uint8_t iter = 0; iter < 4; iter++){
                                if (PID_WriteReg(iter, m_data.reg, m_data.data[iter]) == RW_ERROR){
                                    MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                    MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                                    HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                    HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                    break;
                                }
                            }
                            if (cnt_header.cmd == CMD_ERROR) break;
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 7);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&m_data, sizeof(multiple_reg_data_t), 10);
                            break;
                        case(CMD_READ_REG):
                            memcpy((void *)&m_data, buffer + sizeof(control_header_t), sizeof(multiple_reg_data_t));
                            if (m_data.reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            float tmp = 0;
                            for (uint8_t iter = 0; iter < 4; iter++){
                                if (PID_ReadReg(iter, m_data.reg, &tmp) == RW_ERROR){
                                    MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                    MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                                    HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                    HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                    break;
                                }
                                else m_data.data[iter] = tmp;
                            }
                            if (cnt_header.cmd == CMD_ERROR) break;
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 7);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&m_data, sizeof(multiple_reg_data_t), 10);
                            break;
                        default:
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                            MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            break;
                    }
                    break;
                case(ID_GLOBAL_CMD):
                    switch (cnt_header.cmd){
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
                            MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                            break;
                    }
                    MAKE_BP_HEADER(bp_header.cmd, 2);
                    HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                    HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                    break;
                default:
                    switch(cnt_header.cmd){
                        case(CMD_WRITE_REG):
                            memcpy((void *)&s_data, buffer + sizeof(control_header_t), sizeof(single_reg_data_t));
                            if (s_data.reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            if (PID_WriteReg(cnt_header.id, s_data.reg, s_data.data) == RW_ERROR){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 4);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&s_data, sizeof(single_reg_data_t), 10);
                            break;
                        case(CMD_READ_REG):
                            memcpy((void *)&s_data, buffer + sizeof(control_header_t), sizeof(single_reg_data_t));
                            if (s_data.reg >= REGISTER_LAST){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            float tmp = 0;
                            if (PID_ReadReg(cnt_header.id, s_data.reg, &tmp) == RW_ERROR){
                                MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                                MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                                HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                                break;
                            }
                            else s_data.data = tmp;
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 4);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&s_data, sizeof(single_reg_data_t), 10);
                            break;
                        default:
                            MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
                            MAKE_CONTROL_HEADER(cnt_header.id, CMD_ERROR);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
                            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&cnt_header, sizeof(control_header_t), 10);
                            break;
                    }
                    break;
            }
            break;
        default:
            MAKE_BP_HEADER(BP_ERROR_UNKNOWN_CMD, 0);
            HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t), 10);
            break;
    }
    HAL_UART_Receive_IT(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t));

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
    HAL_UART_Receive_IT(&PROTOCOL_UART, (uint8_t *)&bp_header, sizeof(bp_header_t));
}