#include "systick.h"
#include "ttime.h"

int ttime_elapsed(int now, struct ticks_accumulator *memory, int wait_time)
{
    memory->accumulator += now - memory->previous;
    memory->previous = now;

    int wait = wait_time * TICKS_MULTIPLIER;
    if (memory->accumulator >= wait) // 500 ms
    {
        memory->accumulator %= wait;
        return 1; // time has elapsed 
    }
    
    return 0;
}

void ttime_reset(int now, struct ticks_accumulator *memory)
{
    memory->previous = now;
    memory->accumulator = 0;
}

