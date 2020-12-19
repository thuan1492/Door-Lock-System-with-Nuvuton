#include <stdio.h>
#include <NUC100Series.h>
#include "GPIO.h"
#include "keypad.h"

void KeyPad(void)
{
		//Set PORTA PIN 0->5 is Quasi Bi-Directional
	PA->PMD &= ~(0b11 << 0);
	PA->PMD |= (0b11 << 0);
	
	PA->PMD &= ~(0b11 << 2);
	PA->PMD |= (0b11 << 2);
	
	PA->PMD &= ~(0b11 << 4);
	PA->PMD |= (0b11 << 4);
	
	PA->PMD &= ~(0b11 << 6);
	PA->PMD |= (0b11 << 6);
	
	PA->PMD &= ~(0b11 << 8);
	PA->PMD |= (0b11 << 8);
	
	PA->PMD &= ~(0b11 << 10);
	PA->PMD |= (0b11 << 10);	
	
	//De-bounce for kepad PORTA PIN 0 -> 5
	PA->DBEN &= ~(0b111111 << 0);
	PA->DBEN |= (0b111111 << 0);
}
//return value of pin 0 or 1
int check_pin_value(uint8_t pin_location) {
   return (PA->PIN >> pin_location) & 0b1; // use AND gate to get the value of PIN
}

//Detect the pressed key and return value 1->9
uint8_t key_press(void)
{
	int8_t i=0;
	while(i<3){
		PA->DOUT |= (0b111111 << 0);// set PA0 to PA5 is high acting
		PA->DOUT &= ~(0b1 << (2 - i));// set PA2 to PA0 is low acting in each increment i
		int8_t j=0;
		while(j<3){
			if (check_pin_value(3 + j) == 0) {
				return 1 + i + j * 3;//check button press and return the value
			}
			j++;
		}
		i++;
	}
	return 0;
}

//stay at current key
void key_stay(void){
	uint8_t current_key = key_press();
	while(current_key) {
		current_key = key_press();
	}
}

