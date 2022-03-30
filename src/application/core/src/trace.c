#include "trace.h"

void trace_init(trace *tr, uint32_t ident, uint32_t trigger_level)
{
    rbuff_init(&tr->rb, tr->memory, TRACE_SIZE);

    tr->ident = ident;
    tr->level = trigger_level;
    tr->state = TRACE_IDLE;
    tr->count = 0;

    rbuff_fillzero(&tr->rb);
}

int trace_put(trace *tr, uint16_t in)
{
    if (tr->state == TRACE_FINISHED)
        return -1;

    uint16_t trash;
    rbuff_get_short(&tr->rb, &trash);
    rbuff_put_short(&tr->rb, in);

    tr->count++;

    if (tr->state == TRACE_IDLE)
    {
        // Trigger delay. Have TRACE_DELAY samples before running
        if (tr->count >= TRACE_DELAY)
        {
            tr->count = TRACE_DELAY;
            if (in > tr->level)
            {
                tr->state = TRACE_RUNNING;
            }
        }
    }
    else // (tr->state == TRACE_RUNNING)
    {
        if (tr->count >= TRACE_SIZE)
        {
            tr->state = TRACE_FINISHED;
            return 1;
        }
    }

    return 0;
}

void trace_reset(trace *tr)
{
    tr->count = 0;
    tr->state = TRACE_IDLE;
}




