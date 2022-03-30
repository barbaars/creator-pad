#ifndef _LED_TOGGLE_H_
#define _LED_TOGGLE_H_

#include "events.h"

void led_toggle_init(void);

void led_toggle_play(enum events event, int ticks);
void led_toggle_reverse(enum events event, int ticks);
void led_toggle_forward(enum events event, int ticks);
void led_toggle_stop(enum events event, int ticks);
void led_toggle_record(enum events event, int ticks);
void led_toggle_repeat(enum events event, int ticks);
void led_toggle_bank(enum events event, int ticks);
void led_toggle_special_right(enum events event, int ticks);
void led_toggle_special_left(enum events event, int ticks);

void led_toggle_pad_1(enum events event, int ticks);
void led_toggle_pad_2(enum events event, int ticks);
void led_toggle_pad_3(enum events event, int ticks);
void led_toggle_pad_4(enum events event, int ticks);
void led_toggle_pad_5(enum events event, int ticks);
void led_toggle_pad_6(enum events event, int ticks);
void led_toggle_pad_7(enum events event, int ticks);
void led_toggle_pad_8(enum events event, int ticks);
void led_toggle_pad_9(enum events event, int ticks);
void led_toggle_pad_10(enum events event, int ticks);
void led_toggle_pad_11(enum events event, int ticks);
void led_toggle_pad_12(enum events event, int ticks);

#endif