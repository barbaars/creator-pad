#ifndef _KEYS_H_ 
#define _KEYS_H_

#include "events.h"

typedef enum keys
{
    KEY_REPEAT = 0,
    KEY_REVERSE,
    KEY_FORWARD,
    KEY_RECORD,
    KEY_STOP,
    KEY_PLAY,
    KEY_BANK,
    KEY_NC_7, // nothing connected
    KEY_SPECIAL_RIGHT,
    KEY_SPECIAL_LEFT,
    KEY_END_OF_ENUM
} keys_t;

void keys_task_init(void);
void keys_scan_task(enum events event, int ticks);
void keys_check_task(enum events event, int ticks);
void keys_diagnostic_task(enum events event, int ticks);
int keys_get(keys_t key);
int keys_pressed(void);

#endif