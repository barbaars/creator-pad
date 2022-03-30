#include "critical_section.h"

extern uint32_t critical_section_enter(void);
extern void critical_section_exit(uint32_t pmask);
