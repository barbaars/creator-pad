#include "keys.h"

#include <stdio.h>
#include <string.h>

#include "io_map.h"

const enum events key_to_event[KEY_END_OF_ENUM] = {
    EVENT_KEY_REPEAT_PRESSED,
    EVENT_KEY_REVERSE_PRESSED,
    EVENT_KEY_FORWARD_PRESSED,
    EVENT_KEY_RECORD_PRESSED,
    EVENT_KEY_STOP_PRESSED,
    EVENT_KEY_PLAY_PRESSED,
    EVENT_KEY_BANK_PRESSED,
    EVENT_DUMMY,
    EVENT_KEY_SPECIAL_RIGHT_PRESSED,
    EVENT_KEY_SPECIAL_LEFT_PRESSED,
};

static uint8_t keys_state[KEY_END_OF_ENUM];
static uint8_t keys_state_memory[KEY_END_OF_ENUM];

static volatile int keys_pressed_count = 0;

static void keys_column_read(int row);


void keys_task_init(void)
{
    event_subscribe(EVENT_CYCLIC_1, keys_scan_task);
    event_subscribe(EVENT_CYCLIC_1, keys_check_task);
    event_subscribe(EVENT_CYCLIC_1, keys_diagnostic_task);
}

void keys_scan_task(enum events event, int ticks)
{
    KEY_ROW_0 = 0;
    __NOP();
    __NOP();
    __NOP();
    keys_column_read(0);
    KEY_ROW_0 = 1;

    KEY_ROW_1 = 0;
    __NOP();
    __NOP();
    __NOP();
    keys_column_read(1);
    KEY_ROW_1 = 1;

    KEY_ROW_2 = 0;
    __NOP();
    __NOP();
    __NOP();
    keys_column_read(2);
    KEY_ROW_2 = 1;

    keys_state[KEY_SPECIAL_LEFT] = KEY_INT ^ 1;

    int pressed = 0;
    for (int i=0; i<KEY_END_OF_ENUM; i++)
    {
        pressed += keys_state[i];
    }
    keys_pressed_count = pressed;
}

static void keys_column_read(int row)
{
    int offset = 3 * row;
    
    keys_state[offset++] = KEY_COLUMN_0 ^ 1;
    keys_state[offset++] = KEY_COLUMN_1 ^ 1;
    keys_state[offset++] = KEY_COLUMN_2 ^ 1;
}

void keys_check_task(enum events event, int ticks)
{
    int hit = 0;
    for (int i=0; i<KEY_END_OF_ENUM; i++)
    {
        if (keys_state[i] != keys_state_memory[i])
        {
            if (keys_state[i])
            {
                hit = 1;
                event_publish(key_to_event[i]);
            }
        }
    }

    if (hit)
        event_publish(EVENT_KEY_ANY_PRESSED);

    memcpy(keys_state_memory, keys_state, sizeof(keys_state_memory));
}

void keys_diagnostic_task(enum events event, int ticks)
{
    for (int i=0; i<KEY_END_OF_ENUM; i++)
    {
        if (keys_state[i] != keys_state_memory[i])
        {
            if (keys_state[i])
            {
                printf("key[%d] pressed\n", i);
            }
            else
            {
                printf("key[%d] released\n", i);
            }
        }
    }

    memcpy(keys_state_memory, keys_state, sizeof(keys_state_memory));
}

int keys_get(keys_t key)
{   
    if (key >= KEY_END_OF_ENUM)
        return 0;

    return keys_state[key];
}

int keys_pressed(void)
{
    return keys_pressed_count;
}

