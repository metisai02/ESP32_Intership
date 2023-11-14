#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#define ON true
#define OFF false
typedef struct {
  uint8_t swID;
  uint8_t swData;
}sw_data_t;
typedef enum{
    SWITCH_1 = 11,
    SWITCH_2 = 22,
    SWITCH_3 = 33
}sw_t;

typedef struct 
{
    /* data */
    uint32_t relay;
    bool is_uart;
    bool state;
}control_t;

#endif