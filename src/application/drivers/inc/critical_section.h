#ifndef _CRITICAL_SECTION_H
#define _CRITICAL_SECTION_H

#include "NUC123.h"

__attribute__( ( always_inline ) ) inline uint32_t critical_section_enter(void)
{
    uint32_t pmask = __get_PRIMASK();
    __disable_irq();
    return pmask;
}

__attribute__( ( always_inline ) ) inline void critical_section_exit(uint32_t pmask)
{
    if (!pmask)
        __enable_irq();
}

#endif

