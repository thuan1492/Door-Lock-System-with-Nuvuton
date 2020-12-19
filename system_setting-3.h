#ifndef __SYSTEM_SETTING__
#define __SYSTEM_SETTING__
#include "NUC100Series.h"
extern void init_Sys(void);
extern void Clear_TIMER0(void);
extern void TIMER0_Init(void);
extern void timer0_compare_value(uint32_t ms);
extern void start_counting(void);
extern void EINT1_Init(void);
#endif
