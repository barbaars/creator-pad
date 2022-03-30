#ifndef _PROGRAMS_H_
#define _PROGRAMS_H_

#include "events.h"

void bootloader_check_task_init(void);
void bootloader_check_task(enum events event, int ticks);

void knightrider_task_init(void);
void knightrider_task(enum events event, int ticks);

void cpu_load_task_init(void);
void cpu_load_task(enum events event, int ticks);

void powerdown_task_init(void);
void powerdown_task(enum events event, int ticks);

#endif
