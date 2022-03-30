#ifndef _PADS_H_
#define _PADS_H_

#include "events.h"

typedef enum pads
{
    PAD_1,
    PAD_2,
    PAD_3,
    PAD_4,
    PAD_5,
    PAD_6,
    PAD_7,
    PAD_8,
    PAD_9,
    PAD_10,
    PAD_11,
    PAD_12,
    PAD_END_OF_ENUM
} pads_t;


void pads_as_key_task_init(void);
void pads_as_key_scan_task(enum events event, int ticks);
void pads_as_key_check_task(enum events event, int ticks);

#endif
