#ifndef __LCD__
#define __LCD__
#include "NUC100Series.h"
#define LCD_Xmax 128
#define LCD_Ymax 64
#define FG_COLOR 0xFFFF
#define BG_COLOR 0x0000

extern void init_LCD(void);

extern void init_SPI3(void);

extern void clear_LCD(void);

extern void printC(int16_t x, int16_t y, unsigned char ascii_code);

extern void printC_5x7(int16_t x, int16_t y, unsigned char ascii_code);

extern void print_Line(int8_t line, char text[]);

extern void printS(int16_t x, int16_t y, char text[]);

extern void printS_5x7(int16_t x, int16_t y, char text[]);

extern void Menu_Page(void);
extern void Warning_Page(void);
extern void Code_correct_Page(void);
extern void Change_sucessful_Page(void);
extern void Incorrect_Page(void);
extern void Current_Code_Incorrect_Page(void);
extern void Three_Incorrect_Page(void);
extern void Can_Set_New_Code_Page(void);

#endif

