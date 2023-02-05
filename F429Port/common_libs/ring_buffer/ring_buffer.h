#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <stdint.h>

#define RING_BUFFER_OVERWRITE
// #define RING_BUFFER_USE_MASK
#define RING_BUFFER_THREAD_SAFETY

#define RING_BUFFER_SIZE        500
#define RING_BUFFER_MASK_VALUE  0xA5

typedef struct{
    uint32_t    front;
    uint32_t    back;
    uint32_t    bytes_available;
    uint8_t     buffer[RING_BUFFER_SIZE];
#ifdef RING_BUFFER_THREAD_SAFETY
    void *      semaphore;
#endif
}ring_buffer_t;

typedef enum{
    RING_BUFFER_OK              = 0x00,
    RING_BUFFER_OUT_OF_RANGE    = 0x01,
    RING_BUFFER_NOT_INIT        = 0x02,
    RING_BUFFER_BUSY            = 0x03
}rb_status_e;

#ifdef RING_BUFFER_THREAD_SAFETY
void        rb_init(ring_buffer_t * rb, void * semaphore);
rb_status_e rb_push_data(ring_buffer_t * rb, uint8_t * source, uint32_t size, uint32_t timeout);
rb_status_e rb_take_data(ring_buffer_t * rb, uint8_t * dest, uint32_t size, uint32_t timeout);
void        rb_flush(ring_buffer_t * rb, uint32_t timeout);
rb_status_e rb_popdata(ring_buffer_t * rb, uint32_t timeout);
uint32_t    rb_get_available(ring_buffer_t * rb, uint32_t timeout);
rb_status_e rb_is_available(ring_buffer_t * rb);
#else
void        rb_init(ring_buffer_t * rb);
rb_status_e rb_push_data(ring_buffer_t * rb, uint8_t * source, uint32_t size);
rb_status_e rb_take_data(ring_buffer_t * rb, uint8_t * dest, uint32_t size);
void        rb_flush(ring_buffer_t * rb);
rb_status_e rb_popdata(ring_buffer_t * rb);
uint32_t    rb_get_available(ring_buffer_t * rb);
rb_status_e rb_is_available(ring_buffer_t * rb);
#endif

typedef enum{
    RB_MUTEX_OK     = 0x00,
    RB_MUTEX_ERROR  = 0x01
}rb_mutex_status_e;

#ifdef RING_BUFFER_THREAD_SAFETY
rb_mutex_status_e rb_semaphore_acquire(ring_buffer_t * rb, uint32_t timeout);
#else
rb_mutex_status_e rb_semaphore_acquire(ring_buffer_t * rb);
#endif
rb_mutex_status_e rb_semaphore_release(ring_buffer_t * rb);
rb_mutex_status_e rb_semaphore_is_free(ring_buffer_t * rb);

void              rb_semaphore_released_callback(ring_buffer_t * rb);

#endif