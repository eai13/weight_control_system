#include "protocol.h"
#include "global_config.h"
#include "pid.h"

#include "stm32g071xx.h"
#include "usart.h"

uint8_t Protocol_ProcessFlag = 0;

uint8_t PROTOCOL_MessagePending(void){
    return Protocol_ProcessFlag;
}

void PROTOCOL_ResetPendingFlag(void){
    Protocol_ProcessFlag = 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &PROTOCOL_UART){
        Protocol_ProcessFlag = 1;
        // PROTOCOL_ProcessFrame();
    }
}

static volatile frame_header_t      header;
static volatile single_reg_data_t   s_data;
static volatile multiple_reg_data_t m_data;
static volatile global_cmd_data_t   c_data;
static volatile response_header_t   resp_header;

void PROTOCOL_ProcessFrame(void){
    if (header.id >= ID_LAST){
        resp_header.id = header.id;
        resp_header.status = ERROR_ID;
        HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
        HAL_UART_Receive_IT(&PROTOCOL_UART, &header, sizeof(frame_header_t));
        return;
    }
    resp_header.id = header.id;
    switch(header.id){
        case(ID_GLOBAL):
            switch (header.cmd){
                case (CMD_WRITE_REG):
                    if (HAL_UART_Receive(&PROTOCOL_UART, &m_data, sizeof(multiple_reg_data_t), 10) != HAL_OK){
                        break;
                    }
                    // Check CRC
                    resp_header.status = ERROR_NO_ERROR;
                    for (uint8_t iter = 0; iter < 4; iter++){
                        if (PID_WriteReg(iter, m_data.reg, m_data.data[iter]) == RW_ERROR){
                            resp_header.status = ERROR_WRITE;
                        }
                    }
                    HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
                    break;
                case (CMD_READ_REG):
                    if (HAL_UART_Receive(&PROTOCOL_UART, &m_data, sizeof(multiple_reg_data_t), 10) != HAL_OK)
                    {
                        break;
                    }
                    // Check CRC
                    resp_header.status = ERROR_NO_ERROR;
                    float tmp = 0;
                    for (uint8_t iter = 0; iter < 4; iter++){
                        if (PID_ReadReg(iter, m_data.reg, &tmp) == RW_ERROR){
                            resp_header.status = ERROR_READ;
                        }
                        else m_data.data[iter] = tmp;
                    }
                    
                    // Count CRC
                    HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
                    HAL_UART_Transmit(&PROTOCOL_UART, &m_data, sizeof(multiple_reg_data_t), 10);
                    break;
                default:
                    resp_header.status = ERROR_CMD;
                    HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
                    break;
            }
            break;
        case(ID_GLOBAL_CMD):
            switch (header.cmd){
                case(CMD_BLOCK_DRIVE):
                    // +
                    resp_header.status = ERROR_NO_ERROR;
                    break;
                case(CMD_ENABLE_DRIVE):
                    // +
                    resp_header.status = ERROR_NO_ERROR;
                    break;
                case(CMD_RESET_SYSTEM):
                    // +
                    resp_header.status = ERROR_NO_ERROR;
                    break;
                case(CMD_EXIT_APP):
                    // +
                    resp_header.status = ERROR_NO_ERROR;
                    break;
                default:
                    resp_header.status = ERROR_CMD;
                    break;
            }
            HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
            break;
        default:
            switch (header.cmd){
                case(CMD_READ_REG):
                    if (HAL_UART_Receive(&PROTOCOL_UART, &s_data, sizeof(single_reg_data_t), 10) != HAL_OK){
                        break;
                    }
                    // Check Crc
                    float tmp = 0;
                    if (PID_ReadReg(header.id, s_data.reg, &tmp) == RW_OK){
                        resp_header.status = ERROR_NO_ERROR;
                        s_data.data = tmp;
                    }
                    else resp_header.status = ERROR_READ;
                    // Count Crc
                    HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
                    HAL_UART_Transmit(&PROTOCOL_UART, &s_data, sizeof(single_reg_data_t), 10);
                    break;
                case(CMD_WRITE_REG):
                    if (HAL_UART_Receive(&PROTOCOL_UART, &s_data, sizeof(single_reg_data_t), 10) != HAL_OK){
                        break;
                    }
                    // Check Crc
                    if (PID_WriteReg(header.id, s_data.reg, s_data.data) == RW_OK)
                        resp_header.status = ERROR_NO_ERROR;
                    else resp_header.status = ERROR_WRITE;
                    
                    HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
                    break;
                default:
                    resp_header.status = ERROR_CMD;
                    HAL_UART_Transmit(&PROTOCOL_UART, &resp_header, sizeof(response_header_t), 10);
                    break;
            }
            break;
    }
    HAL_UART_Receive_IT(&PROTOCOL_UART, &header, sizeof(frame_header_t));
}

void PROTOCOL_Start(void){
    HAL_UART_Receive_IT(&PROTOCOL_UART, &header, sizeof(frame_header_t));
}