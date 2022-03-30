#ifndef _TRACE_H_
#define _TRACE_H_

#include <stdint.h>

#include "rbuff.h"

#define TRACE_SIZE 128
#define TRACE_DELAY 10

enum trace_state
{
    TRACE_IDLE,
    TRACE_RUNNING,
    TRACE_FINISHED
};

typedef struct trace
{
    uint32_t ident;
    uint32_t level;
    enum trace_state state;
    uint32_t count;
    rbuff rb;
    uint16_t memory[TRACE_SIZE];
} trace;

static inline void trace_swap(trace** t1, trace** t2)
{
    trace *temp;

    temp = *t1;
    *t1 = *t2;
    *t2 = temp;
}


void trace_init(trace *tr, uint32_t ident, uint32_t trigger_level);
int trace_put(trace *tr, uint16_t in);
void trace_reset(trace *tr);

#endif
