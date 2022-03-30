#include "systick.h"

#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif


int systick_ticks;

void systick_init(void)
{

}

uint32_t systick_load(void)
{
    return 0;
}

void systick_load_reset(void)
{

}

void systick_idle(void)
{
    Sleep(2);
}

uint32_t systick_val(void)
{
    return 0;
}

unsigned int systick_idle_missed(void)
{
    return 0;
}