#include "protocol.h"
#include "global_config.h"
#include "pid.h"
#include "ring_buffer.h"

#include "stm32f429xx.h"
#include "usart.h"
#include "main.h"

#include "cmsis_os.h"
#include "task.h"

#define IRQ_TIMEOUT     0
#define THREAD_TIMEOUT  10

ring_buffer_t rx_rb;
ring_buffer_t rx_reserve_rb;
ring_buffer_t tx_rb;

rb_mutex_status_e rb_semaphore_acquire(ring_buffer_t * rb, uint32_t timeout){
    return osSemaphoreAcquire(rb->semaphore, timeout);
}

rb_mutex_status_e rb_semaphore_release(ring_buffer_t * rb){
    return osSemaphoreRelease(rb->semaphore);
}

rb_mutex_status_e rb_semaphore_is_free(ring_buffer_t * rb){
    if (osSemaphoreGetCount(rb->semaphore)){
        return RB_MUTEX_OK;
    }
    else{
        return RB_MUTEX_ERROR;
    }
}

static inline rb_status_e ReceiveFromRxBuffer(uint8_t * dest, uint32_t size){
    uint32_t timeout = osKernelGetTickCount();
    while(rb_get_available(&rx_rb, IRQ_TIMEOUT) < size){
    // while(rb_take_data(&rx_rb, dest, size) != RING_BUFFER_OK){
        if ((osKernelGetTickCount() - timeout) > PROTOCOL_TIMEOUT){
            return RING_BUFFER_OUT_OF_RANGE;
        }
        osDelay(1);
    }
    rb_take_data(&rx_rb, dest, size, IRQ_TIMEOUT);
    return RING_BUFFER_OK;
}

// osSemaphoreId_t RxSemaphore;
// osSemaphoreId_t TxSemaphore;

// static inline rb_status_e Tx

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

static volatile uint8_t                 tx_buffer[512];
static volatile uint8_t                 rx_reserve_buffer[128];
static volatile uint32_t                rx_reserve_ptr;
static volatile bp_header_t *           p_bp = tx_buffer;
static volatile control_header_t *      p_cnt;
static volatile single_reg_data_t *     p_s_data;
static volatile multiple_reg_data_t *   p_m_data;
static volatile global_cmd_data_t *     p_c_data;
static volatile wc_plottables_t *       p_plottable[4];

uint32_t buffer_flush_timeout;
uint8_t tmp_rx;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &PROTOCOL_UART){
        if (rb_is_available(&rx_rb) == RING_BUFFER_OK){
            // uint32_t available_bytes = rb_get_available(&rx_reserve_rb, IRQ_TIMEOUT);
            // if (available_bytes){
            if (rx_reserve_ptr){
                // rb_take_data(&rx_reserve_rb, rx_reserve_buffer, available_bytes, IRQ_TIMEOUT);
                rb_push_data(&rx_rb, rx_reserve_buffer, rx_reserve_ptr, IRQ_TIMEOUT);//available_bytes, IRQ_TIMEOUT);
                rx_reserve_ptr = 0;
            }
            rb_push_data(&rx_rb, &tmp_rx, 1, IRQ_TIMEOUT);
        }
        else{
            rx_reserve_buffer[rx_reserve_ptr] = tmp_rx;
            rx_reserve_ptr++;
            // rb_push_data(&rx_reserve_rb, &tmp_rx, 1, IRQ_TIMEOUT);
        }

        HAL_UART_Receive_IT(&PROTOCOL_UART, &tmp_rx, 1);
        buffer_flush_timeout = osKernelGetTickCount();
    }
}

osSemaphoreId_t TxSemaphore;
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &PROTOCOL_UART){
        // osSemaphoreRelease(TxSemaphore);
    }
}

// void PROTOCOL_ProcessFrame(void){
//     switch(p_bp->cmd){
//         case(BP_CMD_PING):{
//             MAKE_BP_HEADER(BP_CMD_PING, 1);
//             tx_buffer[5] = 'A'; tx_buffer[6] = 'P'; tx_buffer[7] = 'P'; tx_buffer[8] = '1';
//             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + 4);
//             break;
//         }
//         case(BP_CMD_JUMP):{
//             if (*((uint32_t *)(tx_buffer + sizeof(bp_header_t))) != 0){
//                 MAKE_BP_HEADER(BP_ERROR_UNKNOWN_CMD, 0);
//                 HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t));
//                 break;
//             }
//             MAKE_BP_HEADER(BP_CMD_JUMP, 0);
//             HAL_UART_Transmit(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t), 10);
//             MEMORY_SetActualPosition(ENCODER_1_COUNT, ENCODER_2_COUNT, ENCODER_3_COUNT, ENCODER_4_COUNT, 10);
//             HAL_NVIC_SystemReset();
//         }
//         case(BP_CMD_CONTROL):{
//             p_cnt = (control_header_t *)(tx_buffer + sizeof(bp_header_t));
//             if (p_cnt->id >= ID_LAST){
//                 MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                 MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                 HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                 break;
//             }
//             switch(p_cnt->id){
//                 case(ID_GLOBAL):
//                     switch(p_cnt->cmd){
//                         case(CMD_READ_PLOTTABLE):{
//                             for (uint8_t iter = 0; iter < 4; iter++){
//                                 p_plottable[iter] = (wc_plottables_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(wc_plottables_t) * iter);
//                                 *(p_plottable[iter]) = PID_ReadPlottables(iter);
//                             }
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 30);
//                             MAKE_CONTROL_HEADER(p_cnt->id, p_cnt->cmd);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(wc_plottables_t) * 4);
//                             break;
//                         }
//                         case(CMD_WRITE_REG):{
//                             p_m_data = (multiple_reg_data_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t));
//                             if (p_m_data->reg >= REGISTER_LAST){
//                                 MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                                 MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                                 HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                                 break;
//                             }
//                             for (uint8_t iter = 0; iter < 4; iter++){
//                                 if (PID_WriteReg(iter, p_m_data->reg, p_m_data->data[iter]) == RW_ERROR){
//                                     MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                                     MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                                     HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                                     break;
//                                 }
//                             }
//                             if (p_cnt->cmd == CMD_ERROR) break;
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 7);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(multiple_reg_data_t));
//                             break;
//                         }
//                         case(CMD_READ_REG):{
//                             p_m_data = (multiple_reg_data_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t));
//                             if (p_m_data->reg >= REGISTER_LAST){
//                                 MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                                 MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                                 HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                                 break;
//                             }
//                             float tmp = 0;
//                             for (uint8_t iter = 0; iter < 4; iter++){
//                                 if (PID_ReadReg(iter, p_m_data->reg, &tmp) == RW_ERROR){
//                                     MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                                     MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                                     HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                                     break;
//                                 }
//                                 else p_m_data->data[iter] = tmp;
//                             }
//                             if (p_cnt->cmd == CMD_ERROR) break;
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 7);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(multiple_reg_data_t));
//                             break;
//                         }
//                         case(CMD_CALIBRATE_AS_ZERO_POSITION):{
//                             PID_SetZero(4);
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                             break;
//                         }
//                         case(CMD_STOP_DRIVE):{
//                             PID_StopDrive(4);
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                             break;
//                         }
//                         default:{
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                             MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                             break;
//                         }
//                     }
//                     break;
//                 case(ID_GLOBAL_CMD):
//                     switch (p_cnt->cmd){
//                         case(CMD_BLOCK_DRIVE):{
//                             // +
//                             break;
//                         }
//                         case(CMD_ENABLE_DRIVE):{
//                             // +
//                             break;
//                         }
//                         case(CMD_RESET_SYSTEM):{
//                             // +
//                             break;
//                         }
//                         case(CMD_EXIT_APP):{
//                             // +
//                             break;
//                         }
//                         default:{
//                             MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                             break;
//                         }
//                     }
//                     MAKE_BP_HEADER(p_bp->cmd, 2);
//                     HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                     break;
//                 default:
//                     switch(p_cnt->cmd){
//                         case(CMD_WRITE_REG):{
//                             p_s_data = (single_reg_data_t *)(tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t));
//                             if (p_s_data->reg >= REGISTER_LAST){
//                                 MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                                 MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                                 HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                                 break;
//                             }
//                             if (PID_WriteReg(p_cnt->id, p_s_data->reg, p_s_data->data) == RW_ERROR){
//                                 MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                                 MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                                 HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                                 break;
//                             }
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 4);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(single_reg_data_t));
//                             break;
//                         }
//                         case(CMD_READ_REG):{
//                             p_s_data = tx_buffer + sizeof(bp_header_t) + sizeof(control_header_t);
//                             if (p_s_data->reg >= REGISTER_LAST){
//                                 MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                                 MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                                 HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                                 break;
//                             }
//                             float tmp = 0;
//                             if (PID_ReadReg(p_cnt->id, p_s_data->reg, &tmp) == RW_ERROR){
//                                 MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                                 MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                                 HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                                 break;
//                             }
//                             else p_s_data->data = tmp;
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 4);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t) + sizeof(single_reg_data_t));
//                             break;
//                         }
//                         case(CMD_CALIBRATE_AS_ZERO_POSITION):{
//                             PID_SetZero(p_cnt->id);
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                             break;
//                         }
//                         case(CMD_STOP_DRIVE):{
//                             PID_StopDrive(p_cnt->id);
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                             break;
//                         }
//                         default:{
//                             MAKE_BP_HEADER(BP_CMD_CONTROL, 2);
//                             MAKE_CONTROL_HEADER(p_cnt->id, CMD_ERROR);
//                             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t) + sizeof(control_header_t));
//                             break;
//                         }
//                     }
//                     break;
//             }
//             break;
//         }
//         default:
//             MAKE_BP_HEADER(BP_ERROR_UNKNOWN_CMD, 0);
//             HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, sizeof(bp_header_t));
//             break;
//     }
// }

void PROTOCOL_Start(void){
    HAL_UART_Receive_IT(&PROTOCOL_UART, &tmp_rx, 1);
}

void PROTOCOL_Task(void){
    // TxSemaphore = osSemaphoreNew(1, 1, NULL);
    do{
        void * semaphore_tmp = osSemaphoreNew(1, 1, NULL);
        rb_init(&rx_rb, semaphore_tmp);
        semaphore_tmp = osSemaphoreNew(1, 1, NULL);
        rb_init(&tx_rb, semaphore_tmp);
        semaphore_tmp = osSemaphoreNew(1, 1, NULL);
        rb_init(&rx_reserve_rb, semaphore_tmp);
    }while(0);

    bp_header_t         basic_h;
    control_header_t    control_h;

    buffer_flush_timeout = osKernelGetTickCount();

    PROTOCOL_Start();

    uint32_t bytes_available;

    uint8_t start_bytes_tx[2] = { 0xA5, 0x5A };
    uint8_t start_bytes_rx[2];

    while(1){

        bytes_available = rb_get_available(&tx_rb, IRQ_TIMEOUT);
        // bytes_available = tx_rb.bytes_available;

        // if (osSemaphoreAcquire(TxSemaphore, 0) == osOK){
        if (bytes_available){
            if (rb_take_data(&tx_rb, tx_buffer, bytes_available, IRQ_TIMEOUT) == RING_BUFFER_OK){
                HAL_UART_Transmit(&PROTOCOL_UART, tx_buffer, bytes_available, 100);
            }
        }
        // else{
        //     osSemaphoreRelease(TxSemaphore);
        // }
        // }

        if (ReceiveFromRxBuffer(start_bytes_rx, 2) == RING_BUFFER_OK){
        // if (rb_take_data(&rx_rb, start_bytes_rx, 2) == RING_BUFFER_OK){
            if ((start_bytes_rx[0] != 0xA5) || (start_bytes_rx[1] != 0x5A)){
                rb_flush(&rx_rb, IRQ_TIMEOUT);
                continue;
            }
            if (ReceiveFromRxBuffer((uint8_t *)&basic_h, sizeof(bp_header_t)) == RING_BUFFER_OK){//rb_take_data(&rx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t)) == RING_BUFFER_OK){
                switch(basic_h.cmd){
                    case(BP_CMD_PING):{
                        if (basic_h.w_size == 0){
                            basic_h.w_size = 1;
                            rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                            rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                            rb_push_data(&tx_rb, SELF_ID, 4, IRQ_TIMEOUT);
                        }
                        else{
                            rb_flush(&rx_rb, IRQ_TIMEOUT);
                        }
                        break;
                    }
                    case(BP_CMD_JUMP):{
                        if (basic_h.w_size == 1){
                            uint32_t partition;
                            if (ReceiveFromRxBuffer((uint8_t *)&partition, 4) != RING_BUFFER_OK){
                                rb_flush(&rx_rb, IRQ_TIMEOUT);
                            }
                            else{
                                if (partition == 0){
                                    basic_h.w_size = 0;
                                    HAL_UART_AbortTransmit_IT(&PROTOCOL_UART);
                                    taskENTER_CRITICAL();
                                    HAL_UART_Transmit(&PROTOCOL_UART, start_bytes_tx, 2, 10);
                                    HAL_UART_Transmit(&PROTOCOL_UART, (uint8_t *)&basic_h, sizeof(bp_header_t), 10);
                                    HAL_NVIC_SystemReset();
                                }
                                else{
                                    basic_h.cmd = BP_ERROR_UNKNOWN_CMD;
                                    basic_h.w_size = 0;
                                    rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                    rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                }
                            }
                        }
                        break;
                    }
                    case(BP_CMD_CONTROL):{
                        if (ReceiveFromRxBuffer((uint8_t *)&control_h, sizeof(control_header_t)) != RING_BUFFER_OK){
                            rb_flush(&rx_rb, IRQ_TIMEOUT);
                            break;
                        }
                        if (control_h.id >= ID_LAST){
                            basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                            control_h.cmd = CMD_ERROR;
                            rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                            rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                            rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                            break;
                        }
                        switch(control_h.id){
                            case(ID_GLOBAL):
                                switch(control_h.cmd){
                                    case(CMD_READ_PLOTTABLE):{
                                        wc_plottables_t plottables[4];
                                        for (uint8_t iter = 0; iter < 4; iter++)
                                            plottables[iter] = PID_ReadPlottables(iter);

                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 30;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)plottables, sizeof(wc_plottables_t) * 4, IRQ_TIMEOUT);
                                        break;
                                    }
                                    case(CMD_WRITE_REG):{
                                        multiple_reg_data_t mrd;
                                        if (ReceiveFromRxBuffer((uint8_t *)&mrd, sizeof(multiple_reg_data_t)) != RING_BUFFER_OK){
                                            rb_flush(&rx_rb, IRQ_TIMEOUT);
                                            break;
                                        }
                                        if (mrd.reg >= REGISTER_LAST){
                                            basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                            control_h.cmd = CMD_ERROR;
                                            rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                            break;
                                        }
                                        for (uint8_t iter = 0; iter < 4; iter++){
                                            if (PID_WriteReg(iter, mrd.reg, mrd.data[iter]) == RW_ERROR){
                                                basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                                control_h.cmd = CMD_ERROR;
                                                rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                                rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                                rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                                break;
                                            }
                                        }
                                        if (control_h.cmd == CMD_ERROR) break;
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 7;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&mrd, sizeof(multiple_reg_data_t), IRQ_TIMEOUT);
                                        break;
                                    }
                                    case(CMD_READ_REG):{
                                        multiple_reg_data_t mrd;
                                        if (ReceiveFromRxBuffer((uint8_t *)&mrd, sizeof(multiple_reg_data_t)) != RING_BUFFER_OK){
                                            rb_flush(&rx_rb, IRQ_TIMEOUT);
                                            break;
                                        }
                                        if (mrd.reg >= REGISTER_LAST){
                                            basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                            control_h.cmd = CMD_ERROR;
                                            rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                            break;
                                        }
                                        float tmp = 0;
                                        for (uint8_t iter = 0; iter < 4; iter++){
                                            if (PID_ReadReg(iter, mrd.reg, &tmp) == RW_ERROR){
                                                basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                                control_h.cmd = CMD_ERROR;
                                                rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                                rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                                rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                                break;
                                            }
                                            else mrd.data[iter] = tmp;
                                        }
                                        if (control_h.cmd == CMD_ERROR) break;
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 7;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&mrd, sizeof(multiple_reg_data_t), IRQ_TIMEOUT);
                                        break;
                                    }
                                    case(CMD_CALIBRATE_AS_ZERO_POSITION):{
                                        PID_SetZero(4);
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        break;
                                    }
                                    case(CMD_STOP_DRIVE):{
                                        PID_StopDrive(4);
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        break;
                                    }
                                    default:{
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                        control_h.cmd = CMD_ERROR;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        break;
                                    }
                                }
                                break;
                            case(ID_GLOBAL_CMD):
                                switch (control_h.cmd){
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
                                        control_h.cmd = CMD_ERROR;
                                        break;
                                    }
                                }
                                basic_h.w_size = 2;
                                rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                break;
                            default:
                                switch(control_h.cmd){
                                    case(CMD_WRITE_REG):{
                                        single_reg_data_t srd;
                                        if (ReceiveFromRxBuffer((uint8_t *)&srd, sizeof(single_reg_data_t)) != RING_BUFFER_OK){
                                            rb_flush(&rx_rb, IRQ_TIMEOUT);
                                            break;
                                        }
                                        if (srd.reg >= REGISTER_LAST){
                                            basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                            control_h.cmd = CMD_ERROR;
                                            rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                            break;
                                        }
                                        if (PID_WriteReg(control_h.id, srd.reg, srd.data) == RW_ERROR){
                                            basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                            control_h.cmd = CMD_ERROR;
                                            rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                            break;
                                        }
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 4;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&srd, sizeof(single_reg_data_t), IRQ_TIMEOUT);
                                        break;
                                    }
                                    case(CMD_READ_REG):{
                                        single_reg_data_t srd;
                                        if (ReceiveFromRxBuffer((uint8_t *)&srd, sizeof(single_reg_data_t)) != RING_BUFFER_OK){
                                            rb_flush(&rx_rb, IRQ_TIMEOUT);
                                            break;
                                        }
                                        if (srd.reg >= REGISTER_LAST){
                                            basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                            control_h.cmd = CMD_ERROR;
                                            rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                            break;
                                        }
                                        float tmp = 0;
                                        if (PID_ReadReg(control_h.id, srd.reg, &tmp) == RW_ERROR){
                                            basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                            control_h.cmd = CMD_ERROR;
                                            rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                            break;
                                        }
                                        else srd.data = tmp;
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 4;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&srd, sizeof(single_reg_data_t), IRQ_TIMEOUT);
                                        break;
                                    }
                                    case(CMD_CALIBRATE_AS_ZERO_POSITION):{
                                        PID_SetZero(control_h.id);
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        break;
                                    }
                                    case(CMD_STOP_DRIVE):{
                                        PID_StopDrive(control_h.id);
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        break;
                                    }
                                    default:{
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                        control_h.cmd = CMD_ERROR;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        break;
                                    }
                                }
                                break;
                        }
                        break;
                    }
                    default:{
                        rb_flush(&rx_rb, IRQ_TIMEOUT);
                    }
                }
            }
            else{
                rb_flush(&rx_rb, IRQ_TIMEOUT);
            }
            // else if ((osKernelGetTickCount() - buffer_flush_timeout) > 50)
                // rb_flush(&rx_rb);
        }
        osDelay(5);
    }
}