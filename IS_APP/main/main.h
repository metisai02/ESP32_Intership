#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint8_t swID;
  uint8_t swData;
}sw_data_t;
typedef enum{
    SWITCH_1 = 11,
    SWITCH_2 = 22,
    SWITCH_3 = 33
}sw_t;


#endif