#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include <stdint.h>

#define TICKS_MULTIPLIER 5 // Multiply sys_tics with this number to get 10ms steps

void systick_init(void);
uint32_t systick_load(void);
void systick_load_reset(void);
void systick_idle(void);
uint32_t systick_val(void);
unsigned int systick_idle_missed(void);

#endif
