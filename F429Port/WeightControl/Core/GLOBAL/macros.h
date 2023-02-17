#ifndef __GLOBAL_MACROS__
#define __GLOBAL_MACROS__

#define _TIME_START_(var, timeout)                          \
    uint32_t var##_currenttime = osKernelGetTickCount();    \
    uint32_t var##_period = timeout;
#define _TIME_UPDATE_(var) \
    (var##_currenttime = osKernelGetTickCount())
#define _IS_TIMEOUT_(var) \
    ((osKernelGetTickCount() - var##_currenttime) > var##_period)

#endif