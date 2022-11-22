#include "debug.h"
#include "stdarg.h"
#include "string.h"

// Static buffers for conversion
static int8_t buf1[128];
static int8_t buf2[128];

// Insert the function to use for Debug here
static inline debug_transmit(int8_t * str, uint32_t size){
    HAL_UART_Transmit(&DBG_PORT, str, size, 100);
}

/**
 * @brief for debug print
 * @param str 
 */
void print_db(const char * fmt, ...){
    va_list args;
    va_start(args, fmt);
#ifdef ADD_COLOR_TIMESTAMP
    snprintf(buf1, sizeof(buf1), "[ %d ]\t\033[1m[DEBUG]\033[0m %ls", HAL_GetTick(), fmt);
    vsnprintf(buf2, sizeof(buf2), buf1, args);
#else
    vsnprintf(buf2, sizeof(buf2), fmt, args);
#endif
    debug_transmit(buf2, strlen(buf2));
    va_end(args);
}

/**
 * @brief for warning print
 * @param str 
 */
void print_wr(const char * fmt, ...){
    va_list args;
    va_start(args, fmt);
#ifdef ADD_COLOR_TIMESTAMP
    snprintf(buf1, sizeof(buf1), "[ %d ]\t\033[33;1m[WARN ]\033[0m %ls", HAL_GetTick(), fmt);
    vsnprintf(buf2, sizeof(buf2), buf1, args);
#else
    vsnprintf(buf2, sizeof(buf2), fmt, args);
#endif
    debug_transmit(buf2, strlen(buf2));
    va_end(args);
}

/**
 * @brief for error print
 * @param str 
 */
void print_er(const char * fmt, ...){
    va_list args;
    va_start(args, fmt);
#ifdef ADD_COLOR_TIMESTAMP
    snprintf(buf1, sizeof(buf1), "[ %d ]\t\033[31;1m[ERROR]\033[0m %ls", HAL_GetTick(), fmt);
    vsnprintf(buf2, sizeof(buf2), buf1, args);
#else
    vsnprintf(buf2, sizeof(buf2), fmt, args);
#endif
    debug_transmit(buf2, strlen(buf2));
    va_end(args);
}

/**
 * @brief for info print
 * @param str 
 */
void print_in(const char * fmt, ...){
    va_list args;
    va_start(args, fmt);
#ifdef ADD_COLOR_TIMESTAMP
    snprintf(buf1, sizeof(buf1), "[ %d ]\t\033[32;1m[INFO ]\033[0m %ls", HAL_GetTick(), fmt);
    vsnprintf(buf2, sizeof(buf2), buf1, args);
#else
    vsnprintf(buf2, sizeof(buf2), fmt, args);
#endif
    debug_transmit(buf2, strlen(buf2));
    va_end(args);
}