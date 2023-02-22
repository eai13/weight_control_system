#include "protocol.h"
#include "global_config.h"
#include "pid.h"
#include "ring_buffer.h"
#include "macros.h"
#include "memory.h"

#include "stm32f429xx.h"
#include "iwdg.h"
#include "usart.h"
#include "main.h"

#include "cmsis_os.h"
#include "task.h"

#define IRQ_TIMEOUT     0
#define THREAD_TIMEOUT  10

ring_buffer_t rx_rb;
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
        if ((osKernelGetTickCount() - timeout) > PROTOCOL_TIMEOUT){
            return RING_BUFFER_OUT_OF_RANGE;
        }
        osDelay(1);
    }
    rb_take_data(&rx_rb, dest, size, IRQ_TIMEOUT);
    return RING_BUFFER_OK;
}

static volatile uint8_t                 tx_buffer[512];
static volatile uint8_t                 rx_buffer[512];
static volatile uint32_t                rx_ptr = 0;


uint32_t buffer_ready_timeout;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &PROTOCOL_UART){
        rx_ptr++;
        buffer_ready_timeout = osKernelGetTickCount();
        HAL_UART_Receive_IT(&PROTOCOL_UART, rx_buffer + rx_ptr, 1);
    }
}

osSemaphoreId_t UARTTxSemaphore;
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &PROTOCOL_UART){
        osSemaphoreRelease(UARTTxSemaphore);
    }
}

void PROTOCOL_Start(void){
    rx_ptr = 0;
    HAL_UART_Receive_IT(&PROTOCOL_UART, rx_buffer, 1);
}
void PROTOCOL_Stop(void){
    HAL_UART_AbortReceive_IT(&PROTOCOL_UART);
}

void PROTOCOL_Task(void){
    do{
        void * semaphore_tmp = osSemaphoreNew(1, 1, NULL);
        rb_init(&rx_rb, semaphore_tmp);
        semaphore_tmp = osSemaphoreNew(1, 1, NULL);
        rb_init(&tx_rb, semaphore_tmp);
    }while(0);

    bp_header_t         basic_h;
    control_header_t    control_h;

    UARTTxSemaphore = osSemaphoreNew(1, 1, NULL);

    buffer_ready_timeout = osKernelGetTickCount();

    PROTOCOL_Start();

    uint32_t bytes_available;
    
    const uint8_t start_bytes_tx[2] = { 0xA5, 0x5A };
    uint8_t start_bytes_rx[2];

    _TIME_START_(wd_feed, 500);

    while(1){

        if (_IS_TIMEOUT_(wd_feed) && ((osKernelGetTickCount() - buffer_ready_timeout) < 500)){
            HAL_IWDG_Refresh(&hiwdg);
            _TIME_UPDATE_(wd_feed);
        }

        if (((osKernelGetTickCount() - buffer_ready_timeout) > 3) && (rx_ptr != 0)){
            PROTOCOL_Stop();
            rb_push_data(&rx_rb, rx_buffer, rx_ptr, IRQ_TIMEOUT);
            PROTOCOL_Start();
        }
        bytes_available = rb_get_available(&tx_rb, IRQ_TIMEOUT);
        if (bytes_available){
            if (osSemaphoreAcquire(UARTTxSemaphore, IRQ_TIMEOUT) == osOK){
                if (rb_take_data(&tx_rb, tx_buffer, bytes_available, IRQ_TIMEOUT) == RING_BUFFER_OK){
                    HAL_UART_Transmit_IT(&PROTOCOL_UART, tx_buffer, bytes_available);
                }
                else{
                    osSemaphoreRelease(UARTTxSemaphore);
                }
            }
        }
        
        if (ReceiveFromRxBuffer(start_bytes_rx, 1) == RING_BUFFER_OK){
            if (start_bytes_rx[0] == 0xA5){
                if (ReceiveFromRxBuffer(start_bytes_rx, 1) == RING_BUFFER_OK){
                    if (start_bytes_rx[0] != 0x5A){
                        continue;
                    }
                }
                else{
                    continue;
                }
            }
            else{
                continue;
            }
                
            if (ReceiveFromRxBuffer((uint8_t *)&basic_h, sizeof(bp_header_t)) == RING_BUFFER_OK){
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
                                    taskEXIT_CRITICAL();
                                    MEMORY_SaveAndExit();
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
                                    case(CMD_CALIBRATE_ENCODER_VALUE):{
                                        float data;
                                        if (ReceiveFromRxBuffer((uint8_t *)&data, sizeof(float)) != RING_BUFFER_OK){
                                            break;
                                        }
                                        if (control_h.id > 3){
                                            basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 2;
                                            control_h.cmd = CMD_ERROR;
                                            rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                            rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                            break;
                                        }
                                        data = PID_CalibEncoder(control_h.id, data);
                                        basic_h.cmd = BP_CMD_CONTROL; basic_h.w_size = 3;
                                        rb_push_data(&tx_rb, start_bytes_tx, 2, IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&basic_h, sizeof(bp_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&control_h, sizeof(control_header_t), IRQ_TIMEOUT);
                                        rb_push_data(&tx_rb, (uint8_t *)&data, sizeof(float), IRQ_TIMEOUT);
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
                        break;
                    }
                }
            }
        }
        osDelay(5);
    }
}