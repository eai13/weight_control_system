#include "ring_buffer.h"
#include <stdint.h>
#include <string.h>

#define __weak __attribute__((weak))

void        rb_init(ring_buffer_t * rb
#ifdef RING_BUFFER_THREAD_SAFETY
                    , void * semaphore){
#else
                    ){
#endif
    rb->back = 0;
    rb->front = 0;
    rb->bytes_available = 0;
    memset(rb->buffer, RING_BUFFER_MASK_VALUE, RING_BUFFER_SIZE);
#ifdef RING_BUFFER_THREAD_SAFETY
    rb->semaphore = semaphore;
#endif
}

__weak rb_mutex_status_e rb_semaphore_acquire(ring_buffer_t * rb
#ifdef RING_BUFFER_THREAD_SAFETY
                                              , uint32_t timeout){
    return RB_MUTEX_ERROR;
#else
                                             ){
    return RB_MUTEX_OK;
#endif
}

__weak rb_mutex_status_e rb_semaphore_release(ring_buffer_t * rb){
#ifdef RING_BUFFER_THREAD_SAFETY
    return RB_MUTEX_ERROR;
#else
    return RB_MUTEX_OK;
#endif
}

__weak rb_mutex_status_e rb_semaphore_is_free(ring_buffer_t * rb){
#ifdef RING_BUFFER_THREAD_SAFETY
    return RB_MUTEX_ERROR;
#else
    return RB_MUTEX_OK;
#endif
}

rb_status_e rb_is_available(ring_buffer_t * rb){
    if (rb_semaphore_is_free(rb) == RB_MUTEX_OK){
        return RING_BUFFER_OK;
    }
    else{
        return RING_BUFFER_BUSY;
    }
}

rb_status_e rb_push_data(ring_buffer_t * rb, uint8_t * source, uint32_t size
#ifdef RING_BUFFER_THREAD_SAFETY
                        , uint32_t timeout){
#else
                        ){
#endif
    if ((!size) || (size > RING_BUFFER_SIZE))
        return RING_BUFFER_OUT_OF_RANGE;

#ifdef RING_BUFFER_THREAD_SAFETY
    if (rb_semaphore_acquire(rb, timeout) == RB_MUTEX_OK){
#endif
        if (size == 1){
            rb->buffer[rb->back] = source[0];
            rb->back = (((rb->back + 1) == RING_BUFFER_SIZE) ? (0) : (rb->back + 1));
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
                rb->back = (((rb->back + size) >= RING_BUFFER_SIZE) ? (rb->back + size - RING_BUFFER_SIZE) : (rb->back + size));
                rb->front = rb->back;
                rb->bytes_available = RING_BUFFER_SIZE;
            }
            else{
                rb->back = (((rb->back + size) >= RING_BUFFER_SIZE) ? (rb->back + size - RING_BUFFER_SIZE) : (rb->back + size));
                rb->bytes_available += size;
            }
        }
#ifdef RING_BUFFER_THREAD_SAFETY
        rb_semaphore_release(rb);
#endif
        return RING_BUFFER_OK;
#ifdef RING_BUFFER_THREAD_SAFETY
    }

    return RING_BUFFER_BUSY;
#endif
}

rb_status_e rb_take_data(ring_buffer_t * rb, uint8_t * dest, uint32_t size
#ifdef RING_BUFFER_THREAD_SAFETY
                        , uint32_t timeout){
#else
                        ){
#endif
    if ((!size) || (size > RING_BUFFER_SIZE) || (size > rb->bytes_available))
        return RING_BUFFER_OUT_OF_RANGE;

#ifdef RING_BUFFER_THREAD_SAFETY
    if (rb_semaphore_acquire(rb, timeout) == RB_MUTEX_OK){
#endif
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
#ifdef RING_BUFFER_THREAD_SAFETY
        rb_semaphore_release(rb);
#endif
        return RING_BUFFER_OK;
#ifdef RING_BUFFER_THREAD_SAFETY
    }
    
    return RING_BUFFER_BUSY;
#endif
}

rb_status_e rb_popdata(ring_buffer_t * rb
#ifdef RING_BUFFER_THREAD_SAFETY
                        , uint32_t timeout){
#else
                        ){
#endif

#ifdef RING_BUFFER_THREAD_SAFETY
    if (rb_semaphore_acquire(rb, timeout) == RB_MUTEX_OK){
#endif
        if (rb->bytes_available){
            rb->front = (rb->front + 1) % RING_BUFFER_SIZE;
            rb->bytes_available--;
        }
#ifdef RING_BUFFER_THREAD_SAFETY
        rb_semaphore_release(rb);
#endif
        return RING_BUFFER_OK;
#ifdef RING_BUFFER_THREAD_SAFETY
    }
    return RING_BUFFER_BUSY;
#endif
}

void        rb_flush(ring_buffer_t * rb
#ifdef RING_BUFFER_THREAD_SAFETY
                    , uint32_t timeout){
#else
                    ){
#endif

#ifdef RING_BUFFER_THREAD_SAFETY
    if (rb_semaphore_acquire(rb, timeout) == RB_MUTEX_OK){
#endif
#ifdef RING_BUFFER_USE_MASK
        memset(rb->buffer, RING_BUFFER_MASK_VALUE, RING_BUFFER_SIZE);
#endif
        rb->back = 0;
        rb->front = 0;
        rb->bytes_available = 0;

#ifdef RING_BUFFER_THREAD_SAFETY
        rb_semaphore_release(rb);
#endif
        return RING_BUFFER_OK;
#ifdef RING_BUFFER_THREAD_SAFETY
    }
    return RING_BUFFER_BUSY;
#endif
}

uint32_t rb_get_available(ring_buffer_t * rb
#ifdef RING_BUFFER_THREAD_SAFETY
                            , uint32_t timeout){
#else
                            ){
#endif

#ifdef RING_BUFFER_THREAD_SAFETY
    if (rb_semaphore_acquire(rb, timeout) == RB_MUTEX_OK){
#endif
        uint32_t tmp = rb->bytes_available;
#ifdef RING_BUFFER_THREAD_SAFETY
        rb_semaphore_release(rb);
#endif
        return tmp;
#ifdef RING_BUFFER_THREAD_SAFETY
    }
    return 0;
#endif
}