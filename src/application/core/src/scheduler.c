/* Includes */
#include "scheduler.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "systick.h"

#include "rbuff.h"
#include "cyclic_tasks.h"
#include "events.h"

uint32_t critical_section_enter(void);
void critical_section_exit(uint32_t pmask);

/* Defines */
#define EVENT_QUEUE_SZ     128
#define EVENT_HANDLERS_SZ  128

struct event_index_entry
{
    volatile int index, len;  
};

/* Globals */
extern volatile int systick_ticks;

/* Locals */
static volatile int handlers_called_max;
static volatile int event_queue_full;

static int inited;
static rbuff event_queue;
static uint8_t event_queue_memory[EVENT_QUEUE_SZ];
static int index_free;

// Memory where function poiters to even handlers are placed
static event_handler_t event_handlers[EVENT_HANDLERS_SZ];

// Lookup to corresponding event handlers
static struct event_index_entry event_index[EVENT_MAX];


/* Prototypes */
static int scheduler_run_event(enum events event, int ticks);

/* Functions */

/*
 * scheduler_init
 *
 */
void scheduler_init(void)
{
    rbuff_init(&event_queue, event_queue_memory, EVENT_QUEUE_SZ);

    systick_init();
    cyclic_tasks_init(); 

    systick_load_reset();

    inited = 1;
}

/*
 * scheduler_loop
 *
 */
void scheduler_loop(void)
{
    if (!inited)
        scheduler_init();

    handlers_called_max = 0;
    for (;;)
    {
        int handlers_called = scheduler_check_events(); 
        if (handlers_called > handlers_called_max)
            handlers_called_max = handlers_called;
        
        systick_idle();
    }
}

/*
 * scheduler_check_events
 *
 */
int scheduler_check_events(void)
{
    int handlers_called = 0; 
    for (;;)
    {
        uint32_t c = critical_section_enter();
        enum events event;   
        int result = rbuff_get_byte(&event_queue, (uint8_t *)&event);
        critical_section_exit(c);
        if (result)
            break;

        handlers_called += scheduler_run_event(event, systick_ticks);
    }

    return handlers_called;
}

/*
 * scheduler_run_event
 *
 */
static int scheduler_run_event(enum events event, int ticks)
{
    struct event_index_entry *entry = &event_index[event];

    int i;
    for (i = 0; i < entry->len; i++)
    {
        event_handlers[entry->index + i](event, ticks);
    }

    return i; // Return amount of event handlers called
}

/*
 * scheduler_space
 *
 */
int scheduler_space(void)
{
    return EVENT_HANDLERS_SZ - index_free;
}

/*
 * scheduler_get_diagnostics
 *
 */
const scheduler_diagnostics *scheduler_get_diagnostics(void)
{
    static scheduler_diagnostics diag;

    diag.handlers_called_max = handlers_called_max;
    diag.event_queue_full = event_queue_full;

    return &diag;
}

/*
 * event_subscribe
 *
 */
int event_subscribe(enum events event, event_handler_t handler)
{
    if (index_free >= EVENT_HANDLERS_SZ)
        return -1;

    struct event_index_entry *entry = &event_index[event];

    // Check if the event is already subscribed for.
    if (entry->len == 0)
    {
        // Place handler
        event_handlers[index_free] = handler;

        // Update index
        entry->index = index_free;
    }
    else
    { 
        // Make space for new entry. Move memory after this entry
        int entry_index_end = entry->index + entry->len;
        size_t size = (index_free - entry_index_end) * sizeof(event_handler_t);
        memmove(&event_handlers[entry_index_end+1], &event_handlers[entry_index_end], size);
        
        // Update the index to refelect the move
        for (int i = 0; i < EVENT_MAX; i++)
        {
            if (event_index[i].index > entry->index)
                event_index[i].index++;
        }

        // Place handler
        event_handlers[entry_index_end] = handler;
    }

    entry->len++;
    index_free++;

    return 0;
}

/*
 * event_publish
 *
 */
int event_publish(enum events event)
{
    uint32_t c = critical_section_enter();
    int result = rbuff_put_byte(&event_queue, event);
    if (result)
        event_queue_full++;
    critical_section_exit(c);
    
    return result;
}
