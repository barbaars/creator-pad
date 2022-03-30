#ifndef _LEDS_H_ 
#define _LEDS_H_

#include "events.h"

typedef enum leds 
{ 
    LED_PAD_6,             // 0
    LED_PAD_5,             // 1
    LED_PAD_4,             // 2
    LED_PAD_3,             // 3 
    LED_PAD_2,             // 4
    LED_PAD_1,             // 5
    LED_KEY_SPECIAL_RIGHT, // 6
    LED_PAD_7 = 8,         // 8
    LED_PAD_8,             // 9
    LED_PAD_9,             // 10
    LED_PAD_10,            // 11
    LED_PAD_11,            // 12
    LED_PAD_12,            // 13
    LED_BANK_FB_4 = 16,    // 16
    LED_KEY_REPEAT,        // 17
    LED_KEY_REVERSE,       // 18
    LED_KEY_FORWARD,       // 19
    LED_KEY_RECORD,        // 20
    LED_KEY_STOP,          // 21
    LED_KEY_PLAY = 24,     // 24
    LED_BANK_FB_1,         // 25
    LED_KEY_BANK,          // 26
    LED_KEY_SPECIAL_LEFT,  // 27
    LED_BANK_FB_2,         // 28
    LED_BANK_FB_3,         // 29
    LED_END_OF_ENUM = 32 
} leds_t;

#define LED_ON 1
#define LED_Off 0

void leds_task_init(void);
void leds_task(enum events event, int ticks);

void leds_set(leds_t led, int state);
void leds_toggle(leds_t led);

#endif