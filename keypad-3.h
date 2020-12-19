#ifndef __KEYPAD__
#define __KEYPAD__
#include <NUC100Series.h>
extern void KeyPad(void);

uint8_t key_press(void);
void key_stay(void);
#endif
