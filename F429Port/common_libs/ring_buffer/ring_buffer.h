#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <stdint.h>

#define RING_BUFFER_OVERWRITE
// #define RING_BUFFER_USE_MASK

#define RING_BUFFER_SIZE        512
#define RING_BUFFER_MASK_VALUE  0xA5

typedef struct{
    uint32_t    front;
    uint32_t    back;
    uint32_t    bytes_available;
    uint8_t     buffer[RING_BUFFER_SIZE];
}ring_buffer_t;

typedef enum{
    RING_BUFFER_OK              = 0x00,
    RING_BUFFER_OUT_OF_RANGE    = 0x01,
    RING_BUFFER_NOT_INIT        = 0x02
}rb_status_e;

void        rb_init(ring_buffer_t * rb);
rb_status_e rb_push_data(ring_buffer_t * rb, uint8_t * source, uint32_t size);
rb_status_e rb_take_data(ring_buffer_t * rb, uint8_t * dest, uint32_t size);
void        rb_flush(ring_buffer_t * rb);
rb_status_e rb_popdata(ring_buffer_t * rb);


#endif