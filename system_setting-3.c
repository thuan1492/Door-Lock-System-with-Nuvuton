#include "NUC100Series.h"
#include "system_setting.h"

void init_Sys(void){
	//System initialization start-------------------     
	SYS_UnlockReg();     
	CLK->PWRCON |= (0b1 << 0);
	while(!(CLK->CLKSTATUS & 0b1));
	SYS_LockReg();  // Lock protected registers 
	
	//TMR0 Clock selection and configuration      
	CLK->CLKSEL1 &= ~(0b111 << 8);
	CLK->CLKSEL0 &= ~(0b111 << 0);
	
	// Enable Timer0 clock 
	CLK->APBCLK &= ~(0b1 << 2);
	CLK->APBCLK |= (0b1 << 2); 
	
	//Enable SPI3 clock 
	CLK->APBCLK &= ~(0b1<< 15);
	CLK->APBCLK |= (0b1<< 15);

	//RBG LED
	PA->PMD &= ~(0b11 << 26);
	PA->PMD |= (0b01 << 26);
	
	PA->PMD &= ~(0b11 << 28);
	PA->PMD |= (0b01 << 28);
	
	PA->PMD &= ~(0b1 << 30);
	PA->PMD |= (0b01 << 30);

	// Select SS0, SPICLK, MISO0, MOSI0 function to PORT D PIN 8, 9, 10, 11 respectively.
	// SPI3 SS0 function is selected to PD8
	SYS->GPD_MFP &= ~(0b1 << 8);
	SYS->GPD_MFP |= (0b1 << 8);
	// SPI3 clock function is selected to PD9
	SYS->GPD_MFP &= ~(0b1 << 9);
	SYS->GPD_MFP |= (0b1 << 9);
	// SPI3 MISO0 function is selected to PD10
	SYS->GPD_MFP &= ~(0b1 << 10);
	SYS->GPD_MFP |= (0b1 << 10);
	// SPI3 MOSI0 function is selected to PD11
	SYS->GPD_MFP &= ~(0b1 << 11);
	SYS->GPD_MFP |= (0b1 << 11);
	//System initialization end------------------
	
	// Enable interrupt de-bounce function PA3 -> PA5
	PB->DBEN &= ~ (0b01 << 15);
	PB->DBEN |= (0b01 << 15);
	
	PB->DBEN &= ~ (0b01 << 3);
	PB->DBEN |= (0b01 << 3);
	
	PB->DBEN &= ~ (0b01 << 4);
	PB->DBEN |= (0b01 << 4);
	 
	PB->DBEN &= ~ (0b01 << 5);
	PB->DBEN |= (0b01 << 5);
	
	//LCD back light
	PD->DOUT &= ~(0b1 << 14);
}

void Clear_TIMER0(void) {
	TIMER0->TISR |= (0b1<<0);
}

void TIMER0_Init(void){
	//define Timer 0 operation mode 
	TIMER0->TCSR &= 0;// reset value of registers to 0     
	TIMER0->TCSR |= (0b00 << 27);// One shot mode
	TIMER0->TCSR |= (0b11 << 0); //prescaling 3
	TIMER0->TCSR |= (0b11 << 29); // enable interrupt (bit 29) and enable timer 0 counting (bit 30)
	
	//enable interrupt for Timer 0
  NVIC->ISER[0] &= ~(0b11 << 8);
	NVIC->ISER[0] |= (0b01 << 8);
	
	//set the priority of timer0 to 1
	NVIC->IP[2] &= ~(0b11 << 6);
	NVIC->IP[2] |= (0b01 << 6);
}

void timer0_compare_value(uint32_t ms) {
	//Set TIMER0 compare register value
	TIMER0->TCMPR &= 0;
	TIMER0->TCMPR |= (3000000 / 1000 * ms);
}

void start_counting(void) {
	TIMER0->TCSR |= (0b1 <<30);
}

void EINT1_Init(void){
	// Configure EINT1 pin and enable interrupt by rising and falling edge trigger
	PB->PMD &= ~(0b01 << 30);
	PB->PMD |= (0b00 << 30);
	
	//Set Egde Trigger Interrupt
	PB->IMD &= ~(0b01 << 15);
	PB->IMD |= (0b00 << 15); 
	
	//Set Interrupt enble by falling edge
	PB->IEN &= ~(0b01 << 15);
  PB->IEN |= (0b01 << 15);
	
	// Enable interrupt de-bounce function 
	PB->DBEN &= ~ (0b01 << 15);
	PB->DBEN |= (0b01 << 15);
	
	//enable interrupt for EINT1
  NVIC->ISER[0] &= ~(0b11 << 3);
	NVIC->ISER[0] |= 0b01 << 3;
	//set the priority of EINT1 to 0
	NVIC->IP[0] &= ~(0b01 << 30);
	NVIC->IP[0] |= (0b00 << 30);
	//EINT1 initialization end------------------- 
}
