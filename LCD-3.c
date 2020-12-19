
#include "stdio.h"
#include "string.h"
#include "NUC100Series.h"
#include "SYS.h"
#include "SPI.h"
#include "GPIO.h"	
#include "LCD.h"
#include "Font5x7.h"
#include "Font8x16.h"

char DisplayBuffer[128*8];

void init_SPI3(void)
{
    /* Configure as a master, clock idle low, 9-bit transaction, 
		drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 1MHz */
    SPI_Open(SPI3, SPI_MASTER, SPI_MODE_0, 9, 1000000);
    SPI_DisableAutoSS(SPI3);	
}

void lcdWriteCommand(unsigned char temp)
{
  SPI_SET_SS0_LOW(SPI3);
  SPI_WRITE_TX0(SPI3, temp); // Write Data
  SPI_TRIGGER(SPI3);         // Trigger SPI data transfer           
  while(SPI_IS_BUSY(SPI3));  // Check SPI3 busy status
  SPI_SET_SS0_HIGH(SPI3);	
}

// Wrtie data to LCD 
void lcdWriteData(uint8_t temp)
{
  SPI_SET_SS0_LOW(SPI3);
  SPI_WRITE_TX0(SPI3, 0x100 + temp); // Write Data
  SPI_TRIGGER(SPI3);         // Trigger SPI data transfer           
  while(SPI_IS_BUSY(SPI3));  // Check SPI3 busy status
  SPI_SET_SS0_HIGH(SPI3);
}

// Set Address to LCD
void lcdSetAddr(uint8_t PageAddr, uint8_t ColumnAddr)
{
	// Set PA
  SPI_SET_SS0_LOW(SPI3);
  SPI_WRITE_TX0(SPI3, 0xB0 | PageAddr); // Write Data
  SPI_TRIGGER(SPI3);         // Trigger SPI data transfer           
  while(SPI_IS_BUSY(SPI3));  // Check SPI3 busy status
  SPI_SET_SS0_HIGH(SPI3);
  // Set CA MSB	
  SPI_SET_SS0_LOW(SPI3);
  SPI_WRITE_TX0(SPI3, 0x10 | (ColumnAddr>>4)&0xF); // Write Data
  SPI_TRIGGER(SPI3);         // Trigger SPI data transfer           
  while(SPI_IS_BUSY(SPI3));  // Check SPI3 busy status
  SPI_SET_SS0_HIGH(SPI3);	
  // Set CA LSB
  SPI_SET_SS0_LOW(SPI3);
  SPI_WRITE_TX0(SPI3, 0x00 | (ColumnAddr & 0xF)); // Write Data
  SPI_TRIGGER(SPI3);         // Trigger SPI data transfer           
  while(SPI_IS_BUSY(SPI3));  // Check SPI3 busy status
  SPI_SET_SS0_HIGH(SPI3);
}

void init_LCD(void)
{
	init_SPI3();
	lcdWriteCommand(0xEB); 
	lcdWriteCommand(0x81); 
	lcdWriteCommand(0xA0);  
	lcdWriteCommand(0xC0);  
	lcdWriteCommand(0xAF); // Set Display Enable 
}

void clear_LCD(void)
{
	int16_t i,j;
	for (j=0;j<LCD_Ymax;j++)
	  for (i=0;i<LCD_Xmax;i++)
	     DisplayBuffer[i+j/8*LCD_Xmax]=0;
	
	lcdSetAddr(0x0, 0x0);			  								  
	for (i = 0; i < 132 *8; i++)
	{
		lcdWriteData(0x00);
	}
	lcdWriteData(0x0f);
}

void printC(int16_t x, int16_t y, unsigned char ascii_code)
{
  int8_t i;
  unsigned char temp;	    
  for(i=0;i<8;i++) {
	lcdSetAddr((y/8),(LCD_Xmax+1-x-i));
	temp=Font8x16[(ascii_code-0x20)*16+i];	 
	lcdWriteData(temp);
    }

  for(i=0;i<8;i++) {
	lcdSetAddr((y/8)+1,(LCD_Xmax+1-x-i));	 
	temp=Font8x16[(ascii_code-0x20)*16+i+8];
	lcdWriteData(temp);
    }
}

// print char function using Font5x7
void printC_5x7 (int16_t x, int16_t y, unsigned char ascii_code) 
{
	uint8_t i;
	if (x<(LCD_Xmax-5) && y<(LCD_Ymax-7)) {
	   if      (ascii_code<0x20) ascii_code=0x20;
     else if (ascii_code>0x7F) ascii_code=0x20;
	   else           ascii_code=ascii_code-0x20;
	   for (i=0;i<5;i++) {
			  lcdSetAddr((y/8),(LCD_Xmax+1-x-i)); 
        lcdWriteData(Font5x7[ascii_code*5+i]);
		 }
	}
}

void print_Line(int8_t line, char text[])
{
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC(i*8,line*16,text[i]);
}

void printS(int16_t x, int16_t y, char text[])
{
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC(x+i*8, y,text[i]);
}

void printS_5x7(int16_t x, int16_t y, char text[])
{
	uint8_t i;
	for (i=0;i<strlen(text);i++) {
		printC_5x7(x,y,text[i]);
	  x=x+5;
	}
}
//Draw Pages to LCD
//Menu Page
void Menu_Page(void){
	clear_LCD();
	printS_5x7( 16, 0, "EEET2481 - Group 06");
	printS_5x7( 24, 8, "Door Lock System");
	printS_5x7( 16, 16, "Select the options:");
	printS_5x7( 32, 32, "1. Unlock");
	printS_5x7( 32, 48,"2. Change Key");
	//Turn Off the RGB LED
		PA->DOUT |= (0b1 << 13);
		PA->DOUT |= (0b1 << 14);
		PA->DOUT |= (0b1 << 12);
}


void Warning_Page(void) {
	clear_LCD();
	printS_5x7( 16, 0, "EEET2481 - Group 06");
	printS_5x7( 24, 8, "Door Lock System");
	printS_5x7( 16, 40, "Please Press 1 or 2");
	PA->DOUT ^= (0b1 << 14);//turn on the red LED
}

//Correct Code tittle
void Code_correct_Page(void){
	clear_LCD();
	printS_5x7( 16, 0, "EEET2481 - Group 06");
	printS_5x7( 24, 8, "Door Lock System");
	printS_5x7( 24, 32, "CORRECT KEY CODE!");
	printS_5x7( 32, 40, "DOOR UNLOCKED");
	PA->DOUT ^= (0b1 << 13);//turn on the green LED
}

void Can_Set_New_Code_Page(void){
	clear_LCD();
	printS_5x7( 16, 0, "EEET2481 - Group 06");
	printS_5x7( 24, 8, "Door Lock System");
	printS_5x7( 24, 32, "CORRECT KEY CODE!");
	printS_5x7( 0, 40, " Move to Change Code Page");
	printS_5x7( 32, 48, "in 2 seconds");
	PA->DOUT ^= (0b1 << 13);//turn on the green LED
}

//Change code successful title
void Change_sucessful_Page(void){
	clear_LCD();
	printS_5x7( 16, 0, "EEET2481 - Group 06");
	printS_5x7( 24, 8, "Door Lock System");
	printS_5x7( 8, 40, "CODE CHANGE SUCCESSFUL!");
	PA->DOUT ^= (0b1 << 12);//turn on the blue LED
}

//Incorrect title
void Incorrect_Page(void){
	clear_LCD();
	printS_5x7( 16, 0, "EEET2481 - Group 06");
	printS_5x7( 24, 8, "Door Lock System");
	printS_5x7( 16, 24, "INCORRECT KEY CODE!");
	printS_5x7( 24, 32, "System restarts");
	printS_5x7( 32, 40, "in 1 seccond");
	PA->DOUT ^= (0b1 << 14);//turn on the red LED
}

void Three_Incorrect_Page(void){
	clear_LCD();
	printS_5x7( 16, 0, "EEET2481 - Group 06");
	printS_5x7( 24, 8, "Door Lock System");
	printS_5x7( 8, 24, "3 TIMES INCORRECT CODE!");
	printS_5x7( 24, 32, "System restarts");
	printS_5x7( 32, 40, "in 2 secconds");
}

void Current_Code_Incorrect_Page(void){
	clear_LCD();
	printS_5x7( 16, 0, "EEET2481 - Group 06");
	printS_5x7( 24, 8, "Door Lock System");
	printS_5x7( 16, 32, "INCORRECT KEY CODE!");
	printS_5x7( 24, 40, "Please Try again");
	PA->DOUT ^= (0b1 << 14);//turn on the red LED
}

