#include "cyclic_tasks.h"

#include <stdint.h>
#include <stdio.h>

#include "events.h"
#include "programs.h"


#define CYCLIC_CLASSES 8


struct cyclic_class
{
    int time;
    int last;
    unsigned int counter;
    enum events event;
};

struct cyclic_class cyclic_classes[CYCLIC_CLASSES] = {
    {1,    0, 0, EVENT_CYCLIC_1}, // 2ms
    {5,    0, 0, EVENT_CYCLIC_2}, // 10ms
    {25,   0, 0, EVENT_CYCLIC_3}, // 50ms
    {40,   0, 0, EVENT_CYCLIC_4}, // 80ms
    {50,   0, 0, EVENT_CYCLIC_5}, // 100ms
    {250,  0, 0, EVENT_CYCLIC_6}, // 500ms
    {500,  0, 0, EVENT_CYCLIC_7}, // 1s
    {5000, 0, 0, EVENT_CYCLIC_8}  // 10s
};

/*
 * By adding some primenumber offsets to task trigger time 
 * we prevent some tasks to be triggered at the same time 
 * see scheduler_init()
 */
const int cyclic_offsets[CYCLIC_CLASSES] = {0, 1, 3, 5, 11, 23, 53, 499};

static void cyclic_tasks(enum events e, int ticks);

void cyclic_tasks_init(void)
{
    // Subscibe to systick
    event_subscribe(EVENT_SYSTICK, cyclic_tasks);  

    for (int i=0; i<CYCLIC_CLASSES; i++)
        cyclic_classes[i].last = -(int32_t)cyclic_offsets[i];
}

static void cyclic_tasks(enum events e, int ticks)
{
    event_publish(cyclic_classes[0].event);

    for (int i=1; i<CYCLIC_CLASSES; i++)
    {
        int delta = ticks-cyclic_classes[i].last;

        if (delta >= cyclic_classes[i].time)
        {
            cyclic_classes[i].last = ticks - (cyclic_classes[i].time - delta); // Correction
            cyclic_classes[i].counter++;
            event_publish(cyclic_classes[i].event);
        }
    }
}
