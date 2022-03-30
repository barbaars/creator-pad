#ifndef _ANALOG_INPUTS_H_ 
#define _ANALOG_INPUTS_H_

#include <stdint.h>
#include "events.h"

typedef enum analog_inputs 
{ 
    AI_PAD_1,       // 0
    AI_PAD_7,       // 1
    AI_PAD_12,      // 2
    AI_PAD_2,       // 3 
    AI_PAD_10,      // 4
    AI_PAD_8,       // 5
    AI_PAD_9,       // 6
    AI_PAD_11,      // 7
    AI_PAD_3,       // 8
    AI_PAD_4,       // 9
    AI_PAD_5,       // 10
    AI_PAD_6,       // 11
    AI_SLIDER,      // 12
    ANALOG_INPUTS
} analog_inputs_t;

void analog_inputs_init(void);
uint32_t analog_inputs_get(analog_inputs_t input);

void analog_inputs_task(enum events event, int ticks);
void analog_inputs_diag_task(enum events event, int ticks);

uint32_t analog_inputs_adc_rate(void);
uint32_t analog_inputs_pwm_rate(void);


#endif
