#include "leds.h"

#include <string.h>

#include "io_map.h"




/* Variables */
static uint8_t leds_state[LED_END_OF_ENUM];
static volatile int row;

/* Prototypes */
static void leds_column_set(int row);

/* Functions */
void leds_task_init(void)
{
    row = 0;
    memset(leds_state, LED_Off, LED_END_OF_ENUM);

    event_subscribe(EVENT_CYCLIC_1, leds_task);
}

void leds_task(enum events event, int ticks)
{
    LED_ROW_0 = 1;
    LED_ROW_1 = 1;
    LED_ROW_2 = 1;
    LED_ROW_3 = 1;

    switch (row)
    {
        default:
            row = 0;
        case 0:
            leds_column_set(row);
            LED_ROW_0 = 0;
            break;
        case 1:
            leds_column_set(row);
            LED_ROW_1 = 0;
            break;
        case 2:
            leds_column_set(row);
            LED_ROW_2 = 0;
            break;
        case 3:
            leds_column_set(row);
            LED_ROW_3 = 0;
            break;
    }
    row++;
}

static void leds_column_set(int row)
{
    int offset = 8 * row;
    
    LED_COLUMN_0 = leds_state[offset++] ^ 1;
    LED_COLUMN_1 = leds_state[offset++] ^ 1;
    LED_COLUMN_2 = leds_state[offset++] ^ 1;
    LED_COLUMN_3 = leds_state[offset++] ^ 1;
    LED_COLUMN_4 = leds_state[offset++] ^ 1;
    LED_COLUMN_5 = leds_state[offset++] ^ 1;
    LED_COLUMN_6 = leds_state[offset++] ^ 1;
    LED_COLUMN_7 = leds_state[offset++] ^ 1;
}

void leds_set(leds_t led, int state)
{
    if (led >= LED_END_OF_ENUM)
        return;

    leds_state[led] = state & 0xFF;
}

void leds_toggle(leds_t led)
{
    if (led >= LED_END_OF_ENUM)
        return;
        
    leds_state[led] ^= 1; 
}
