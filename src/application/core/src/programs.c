#include "programs.h"

#include <stdio.h>

#include "systick.h"
#include "usb.h"
#include "keys.h"
#include "leds.h"
#include "analog_inputs.h"
#include "bootloader_run.h"
#include "powerdown.h"

#include "ttime.h"
#include "rbuff.h"

void bootloader_check_task_init(void)
{
    event_subscribe(EVENT_CYCLIC_5, bootloader_check_task);
}

void bootloader_check_task(enum events event, int ticks)
{
    static struct ticks_accumulator accumulator = {0};

    if (keys_get(KEY_SPECIAL_LEFT) && keys_get(KEY_SPECIAL_RIGHT) && keys_pressed()==2)
    {
        if (ttime_elapsed(ticks, &accumulator, 400))
        {
            bootloader_run();
        }
    }
    else
    {
        ttime_reset(ticks, &accumulator);
    }
}

void knightrider_task_init(void)
{
    event_subscribe(EVENT_CYCLIC_7, knightrider_task);
}

void knightrider_task(enum events event, int ticks)
{
    static struct ticks_accumulator accumulator = {0};

    if (!ttime_elapsed(ticks, &accumulator, 50)) // check if 500 ms has elapsed 
        return;

    leds_set(LED_BANK_FB_1, LED_Off);
    leds_set(LED_BANK_FB_2, LED_Off);
    leds_set(LED_BANK_FB_3, LED_Off);
    leds_set(LED_BANK_FB_4, LED_Off);

    static int count = 0;
    static int direction = 0;

    switch (count)
    {
        default:
            count = 0;
        case 0:
            leds_set(LED_BANK_FB_1, LED_ON);
            direction = 1;
            break;
        case 1:
            leds_set(LED_BANK_FB_2, LED_ON);
            break;
        case 2:
            leds_set(LED_BANK_FB_3, LED_ON);
            break;
        case 3:
            leds_set(LED_BANK_FB_4, LED_ON);
            direction = -1;
            break;
    }
    count += direction;
}

rbuff cpu_load_history;

#define CPU_LOAD_HISTORY_SZ 64
uint16_t cpu_load_mem[CPU_LOAD_HISTORY_SZ];

void cpu_load_task_init(void)
{
    // Initialize cpu_load history buffer and fill with zeros
    rbuff_init(&cpu_load_history, cpu_load_mem, CPU_LOAD_HISTORY_SZ);
    int result;
    do
    {
        result = rbuff_put_short(&cpu_load_history, 0);
    } 
    while(result == 0);

    event_subscribe(EVENT_CYCLIC_8, cpu_load_task);
}

void cpu_load_task(enum events event, int ticks)
{
    int cpu_load = systick_load();
    systick_load_reset();

    printf("cpuload: %d\n", cpu_load);

    uint16_t trash;
    rbuff_get_short(&cpu_load_history, &trash);
    rbuff_put_short(&cpu_load_history, (uint16_t)(cpu_load&0xFFFF));
}

void powerdown_task_init(void)
{
    event_subscribe(EVENT_CYCLIC_6, powerdown_task);
    event_subscribe(EVENT_KEY_ANY_PRESSED, powerdown_task);
    event_subscribe(EVENT_PAD_ANY_PRESSED, powerdown_task);
}

void powerdown_task(enum events event, int ticks)
{
    static struct ticks_accumulator accumulator = {0};

    if (usb_attached())
    {
        ttime_reset(ticks, &accumulator);
        return;
    }

    if (event == EVENT_KEY_ANY_PRESSED || event == EVENT_PAD_ANY_PRESSED)
    {
        ttime_reset(ticks, &accumulator);
        return;
    }

    if (!ttime_elapsed(ticks, &accumulator, 6000)) // check if 60 seconds have passed without any presses
        return;

    powerdown();
}
