#include "pads.h"

#include <stdint.h>
#include <string.h>

#include "analog_inputs.h"

#define PADS_HIGH_LEVEL   220
#define PADS_LOW_LEVEL    110

const enum analog_inputs pad_to_analog_input[PAD_END_OF_ENUM] = {
    AI_PAD_1,
    AI_PAD_2,
    AI_PAD_3,
    AI_PAD_4,
    AI_PAD_5,
    AI_PAD_6,
    AI_PAD_7,
    AI_PAD_8,
    AI_PAD_9,
    AI_PAD_10,
    AI_PAD_11,
    AI_PAD_12,
};

const enum events pad_as_key_to_event[PAD_END_OF_ENUM] = {
    EVENT_PAD_1_PRESSED,
    EVENT_PAD_2_PRESSED,
    EVENT_PAD_3_PRESSED,
    EVENT_PAD_4_PRESSED,
    EVENT_PAD_5_PRESSED,
    EVENT_PAD_6_PRESSED,
    EVENT_PAD_7_PRESSED,
    EVENT_PAD_8_PRESSED,
    EVENT_PAD_9_PRESSED,
    EVENT_PAD_10_PRESSED,
    EVENT_PAD_11_PRESSED,
    EVENT_PAD_12_PRESSED,
};



static uint8_t pads_state[PAD_END_OF_ENUM];
static uint8_t pads_state_memory[PAD_END_OF_ENUM];

void pads_as_key_task_init(void)
{
    event_subscribe(EVENT_CYCLIC_2, pads_as_key_scan_task);
    event_subscribe(EVENT_CYCLIC_2, pads_as_key_check_task);
}

void pads_as_key_scan_task(enum events event, int ticks)
{
    for (int i=0; i<PAD_END_OF_ENUM; i++)
    {
        uint32_t analog_value = analog_inputs_get(pad_to_analog_input[i]);
        if (analog_value > PADS_HIGH_LEVEL)
        {
            pads_state[i] = 1;
        }
        else if (analog_value < PADS_LOW_LEVEL)
        {
            pads_state[i] = 0;
        }
    }
}

void pads_as_key_check_task(enum events event, int ticks)
{
    int hit = 0;
    for (int i=0; i<PAD_END_OF_ENUM; i++)
    {
        if (pads_state[i] != pads_state_memory[i])
        {
            if (pads_state[i])
            {
                hit = 1;
                event_publish(pad_as_key_to_event[i]);
            }
        }
    }

    if (hit)
        event_publish(EVENT_PAD_ANY_PRESSED);

    memcpy(pads_state_memory, pads_state, sizeof(pads_state_memory));
}