#include "ring_buffer.h"
#include <stdint.h>
#include <string.h>

void        rb_init(ring_buffer_t * rb){
    rb->back = 0;
    rb->front = 0;
    rb->bytes_available = 0;
    memset(rb->buffer, RING_BUFFER_MASK_VALUE, RING_BUFFER_SIZE);
}

rb_status_e rb_push_data(ring_buffer_t * rb, uint8_t * source, uint32_t size){
    if ((!size) || (size > RING_BUFFER_SIZE))
        return RING_BUFFER_OUT_OF_RANGE;

    if (size == 1){
        rb->buffer[rb->back] = source[0];
        rb->back = (((rb->back + 1) == RING_BUFFER_SIZE) ? (0) : (rb->back + 1));//(rb->back + 1) % RING_BUFFER_SIZE;
        if (rb->bytes_available < RING_BUFFER_SIZE) rb->bytes_available++;
        else rb->front = (rb->front + 1) % RING_BUFFER_SIZE;
    }
    else if ((RING_BUFFER_SIZE - rb->back) < size){
        uint32_t size_1 = RING_BUFFER_SIZE - rb->back;
        uint32_t size_2 = size - size_1;
        memcpy(rb->buffer + rb->back, source, size_1);
        memcpy(rb->buffer, source + size_1, size_2);
        rb->back = size_2;
        if (rb->front < size_2) rb->front = size_2;
        if ((rb->bytes_available + size) > RING_BUFFER_SIZE){
            rb->bytes_available = RING_BUFFER_SIZE;
        }
        else{
            rb->bytes_available += size;
        }
    }
    else{
        memcpy(rb->buffer + rb->back, source, size);
        if (rb->bytes_available + size > RING_BUFFER_SIZE){
            rb->back += size;
            rb->front = rb->back;
            rb->bytes_available = RING_BUFFER_SIZE;
        }
        else{
            rb->back += size;
            rb->bytes_available += size;
        }
    }

    return RING_BUFFER_OK;
}

rb_status_e rb_take_data(ring_buffer_t * rb, uint8_t * dest, uint32_t size){
    if ((!size) || (size > RING_BUFFER_SIZE) || (size > rb->bytes_available))
        return RING_BUFFER_OUT_OF_RANGE;

    if (size == 1){
        dest[0] = rb->buffer[rb->front];
        rb->front = (rb->front + 1) % RING_BUFFER_SIZE;
        rb->bytes_available--;
    }
    else if ((RING_BUFFER_SIZE - rb->front) < size){
        uint32_t size_1 = RING_BUFFER_SIZE - rb->front;
        uint32_t size_2 = size - size_1;
        memcpy(dest, rb->buffer + rb->front, size_1);
        memcpy(dest + size_1, rb->buffer, size_2);
        rb->front = size_2;
        rb->bytes_available -= size;
    }
    else{
        memcpy(dest, rb->buffer + rb->front, size);
        rb->front = (rb->front + size) % RING_BUFFER_SIZE;
        rb->bytes_available -= size;
    }
    
    return RING_BUFFER_OK;
}

rb_status_e rb_popdata(ring_buffer_t * rb){
    if (rb->bytes_available){
        rb->front = (rb->front + 1) % RING_BUFFER_SIZE;
        rb->bytes_available--;
    }
}

void        rb_flush(ring_buffer_t * rb){
#ifdef RING_BUFFER_USE_MASK
    memset(rb->buffer, RING_BUFFER_MASK_VALUE, RING_BUFFER_SIZE);
#endif
    rb->back = 0;
    rb->front = 0;
    rb->bytes_available = 0;
}