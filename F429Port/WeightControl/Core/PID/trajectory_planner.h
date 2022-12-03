#ifndef TRAJECTORY_PLANNER_H
#define TRAJECTORY_PLANNER_H

#include <stdint.h>

#pragma pack(push, 1)
typedef struct{
    float       position;
    uint16_t    ticks;
}trajectory_point;
#pragma pack(pop)

trajectory_point trajectory_data[100] = {0, }




#endif