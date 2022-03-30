#include "led_toggle.h"

#include "leds.h"

void led_toggle_init(void)
{
    event_subscribe(EVENT_KEY_PLAY_PRESSED, led_toggle_play);
    event_subscribe(EVENT_KEY_REVERSE_PRESSED, led_toggle_reverse);
    event_subscribe(EVENT_KEY_FORWARD_PRESSED, led_toggle_forward);
    event_subscribe(EVENT_KEY_STOP_PRESSED, led_toggle_stop);
    event_subscribe(EVENT_KEY_RECORD_PRESSED, led_toggle_record);
    event_subscribe(EVENT_KEY_REPEAT_PRESSED, led_toggle_repeat);
    event_subscribe(EVENT_KEY_BANK_PRESSED, led_toggle_bank);
    event_subscribe(EVENT_KEY_SPECIAL_RIGHT_PRESSED, led_toggle_special_right);
    event_subscribe(EVENT_KEY_SPECIAL_LEFT_PRESSED, led_toggle_special_left);

    event_subscribe(EVENT_PAD_1_PRESSED, led_toggle_pad_1);
    event_subscribe(EVENT_PAD_2_PRESSED, led_toggle_pad_2);
    event_subscribe(EVENT_PAD_3_PRESSED, led_toggle_pad_3);
    event_subscribe(EVENT_PAD_4_PRESSED, led_toggle_pad_4);
    event_subscribe(EVENT_PAD_5_PRESSED, led_toggle_pad_5);
    event_subscribe(EVENT_PAD_6_PRESSED, led_toggle_pad_6);
    event_subscribe(EVENT_PAD_7_PRESSED, led_toggle_pad_7);
    event_subscribe(EVENT_PAD_8_PRESSED, led_toggle_pad_8);
    event_subscribe(EVENT_PAD_9_PRESSED, led_toggle_pad_9);
    event_subscribe(EVENT_PAD_10_PRESSED, led_toggle_pad_10);
    event_subscribe(EVENT_PAD_11_PRESSED, led_toggle_pad_11);
    event_subscribe(EVENT_PAD_12_PRESSED, led_toggle_pad_12);
}

void led_toggle_play(enum events event, int ticks)
{
    leds_toggle(LED_KEY_PLAY);
}

void led_toggle_reverse(enum events event, int ticks)
{
    leds_toggle(LED_KEY_REVERSE);
}

void led_toggle_forward(enum events event, int ticks)
{
    leds_toggle(LED_KEY_FORWARD);
}

void led_toggle_stop(enum events event, int ticks)
{
    leds_toggle(LED_KEY_STOP);
}

void led_toggle_record(enum events event, int ticks)
{
    leds_toggle(LED_KEY_RECORD);
}

void led_toggle_repeat(enum events event, int ticks)
{
    leds_toggle(LED_KEY_REPEAT);
}

void led_toggle_bank(enum events event, int ticks)
{
    leds_toggle(LED_KEY_BANK);
}

void led_toggle_special_right(enum events event, int ticks)
{
    leds_toggle(LED_KEY_SPECIAL_RIGHT);
}

void led_toggle_special_left(enum events event, int ticks)
{
    leds_toggle(LED_KEY_SPECIAL_LEFT);
} 

void led_toggle_pad_1(enum events event, int ticks)
{
    leds_toggle(LED_PAD_1);
}

void led_toggle_pad_2(enum events event, int ticks)
{
    leds_toggle(LED_PAD_2);
}

void led_toggle_pad_3(enum events event, int ticks)
{
    leds_toggle(LED_PAD_3);
}

void led_toggle_pad_4(enum events event, int ticks)
{
    leds_toggle(LED_PAD_4);
}

void led_toggle_pad_5(enum events event, int ticks)
{
    leds_toggle(LED_PAD_5);
}

void led_toggle_pad_6(enum events event, int ticks)
{
    leds_toggle(LED_PAD_6);
}

void led_toggle_pad_7(enum events event, int ticks)
{
    leds_toggle(LED_PAD_7);
}

void led_toggle_pad_8(enum events event, int ticks)
{
    leds_toggle(LED_PAD_8);
}

void led_toggle_pad_9(enum events event, int ticks)
{
    leds_toggle(LED_PAD_9);
}

void led_toggle_pad_10(enum events event, int ticks)
{
    leds_toggle(LED_PAD_10);
}

void led_toggle_pad_11(enum events event, int ticks)
{
    leds_toggle(LED_PAD_11);
}

void led_toggle_pad_12(enum events event, int ticks)
{
    leds_toggle(LED_PAD_12);
}
