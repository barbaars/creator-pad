#ifndef _TTIME_H_H
#define _TTIME_H_H

struct ticks_accumulator
{
    int previous;
    int accumulator;
};

int ttime_elapsed(int now, struct ticks_accumulator *memory, int wait_time);
void ttime_reset(int now, struct ticks_accumulator *memory);

#endif
