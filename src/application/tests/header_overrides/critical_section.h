#ifndef _CRITICAL_SECTION_H
#define _CRITICAL_SECTION_H

#include <stdint.h>

__attribute__( ( always_inline ) ) inline uint32_t critical_section_enter(void)
{
    return 0;
}

__attribute__( ( always_inline ) ) inline void critical_section_exit(uint32_t pmask)
{

}

#endif

