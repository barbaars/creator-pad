#include "systick.h"

#include "NUC123.h"

#include "events.h"

/* Globals */
volatile int systick_ticks;

/* Locals */
static volatile uint32_t idle_time_entry; // Systick counter before entering sleep
static volatile uint32_t idle_time_total; // Amount of systicks spend sleeping
static volatile uint32_t idle_time_memory; // Remember the lowest amount of systicks spend sleeping

static volatile unsigned int idle_missed;
static volatile uint32_t idle_flag;

/* Functions */

/*
 * INTERRUPT: SysTick_Handler
 *
 */
void SysTick_Handler(void)
{
    // Remember the lowest idle time. (most cpu usage)
    if (idle_time_total < idle_time_memory) 
        idle_time_memory = idle_time_total;
    idle_time_total = 0;
    idle_time_entry = SysTick->LOAD;

    systick_ticks++;
    event_publish(EVENT_SYSTICK);

    if (!idle_flag)
    {
        idle_missed++;
    }
    idle_flag = 0;
}

/*
 * systick_init
 *
 */
void systick_init(void)
{
    SysTick_Config(SystemCoreClock / (100 * TICKS_MULTIPLIER)); // Interrupt every 10ms/TICKS_MULTIPLIER  

    systick_ticks = 0;
    idle_missed = 0;
}

/*
 * systick_load
 * 
 *  Return system load 0 to 1000 = 0.0 to 100.0 procent cpu usage
 */
uint32_t systick_load(void)
{
    uint32_t load = 1000 - (idle_time_memory * 1000 / SysTick->LOAD);
    return load;
}

/*
 * systick_load_reset
 *
 */
void systick_load_reset(void)
{
    idle_time_memory = SysTick->LOAD;
}

/*
 * systick_idle
 *
 */
void systick_idle(void)
{
    // Set flag that idle has been reached this cycle
    idle_flag = 1;

    // Check how  long we are sleeping using SysTick value
    idle_time_entry = SysTick->VAL;
    
    uint32_t pmask = __get_PRIMASK();
    __disable_irq();
    __DSB(); // is this neccessary ?
    __WFI();
    __ISB(); // is this neccessary ?

    // When woken up with en interupt. It will first update idle_time_total before interupt handler is called after __enable_irq() 
    idle_time_total += idle_time_entry - SysTick->VAL;

    __NOP(); // is this neccessary ?

    if (!pmask)
        __enable_irq();
}

/*
 * systick_val
 *
 */
uint32_t systick_val(void)
{
    return SysTick->LOAD - SysTick->VAL;
}

/*
 * systick_idle_missed
 *
 */
unsigned int systick_idle_missed(void)
{
    return idle_missed;
}