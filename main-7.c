/****
EEET2481 - Embedded System Design and Implementation
Lecturer: Andrew Smith
Group 06
Group Members:

Nguyen Thanh Thuan		s3517236		s3517236@rmit.edu.vn
Le An Nguyen					s3651589		s3651589@rmit.edu.vn
Vo Hoang Dao 					s3634816		s3634816@rmit.edu.vn

Due Date: 20/05/2019
*/

#include <stdio.h>
#include "NUC100Series.h"
#include "LCD.h"
#include "keypad.h"
#include "system_setting.h"

#define MENU_PAGE 0
#define UNLOCK_PAGE 1
#define UNLOCK_CHANGE_CODE_PAGE 2
#define CHANGE_CODE_PAGE 3
#define CORRECT_PAGE 4
#define INCORRECT_PAGE 5
#define MOVE_PAGE 6
#define WARNING_PAGE 7
#define CURRENT_CODE_INCORRECT_PAGE 8
#define PRESS 1
#define NO_PRESS 0

int8_t check_typing = 0;
int8_t type_status = PRESS;
int8_t i, x;
int8_t current_key=0;
int8_t incorrect_code_count = 0;
int8_t page = 0;

volatile int button_flag = 1;
volatile char code_count[6] = {'_', '_', '_', '_', '_', '_'};
char default_code[6] = {'1', '2', '3', '4', '5', '6'};
char current_code[6];// store the typing code

//Set new 6 digits page
void Change_Page(void) {
	clear_LCD();
	printS_5x7( 16, 0, "EEET2481 - Group 06");
	printS_5x7( 24, 8, "Door Lock System");
	printS_5x7( 16, 24, "Enter a new 6 digits");
	x=0;
	i=0; 
	//underscore showing
	while(i<6){
		printC_5x7(x ,54 , code_count[i]);
		x = x + 24;
		i++;
	}
}

//Enter current code page
void Check_Code_Page(void) {
	clear_LCD();
	printS_5x7( 16, 0, "EEET2481 - Group 06");
	printS_5x7( 24, 8, "Door Lock System");
	printS_5x7( 16, 24, "Enter the key code");
	x=0;
	i=0;
	//underscore showing
	while(i<6){
		printC_5x7(x ,54 , code_count[i]);
		x = x + 24;
		i++;
	}
}

//reset code to underscore
void reset_code_count(void) {
	i = 0;
	while(i<6){
		code_count[i] = '_';
		i++;
	}
}

//store new code to default_code array
void store_new_code(void) {
	i = 0;
	while(i<6){
		default_code[i] = current_code[i];
		i++;
	}
}

//reset keycode typing
void reset_typing(void){
	check_typing = 0;
	type_status = PRESS;
	reset_code_count();
}

//store input key code 
void typing(void){
	current_code[check_typing] = '0' + current_key;
	code_count[check_typing++] = '0' + current_key;
	type_status = NO_PRESS;
	key_stay();
	timer0_compare_value(200);// the code typing display in 200ms
	start_counting();
}

void Delay_s(uint32_t count) {
	uint32_t i;
	 for (i = 0; i < count; i++) {
	 }
} 

// main program
int main(void)
{
	//Initialize Clock source, EINT1 (PB15) and TIMER0
	init_Sys();
	EINT1_Init();
	TIMER0_Init();
	//Initialize LCD and Clear LCD
  init_LCD();
  clear_LCD();
	//Initialize keypad and set Menu Page is default page
	KeyPad();
	Menu_Page();
	
	while(1) {
		switch (page) {
			case MENU_PAGE:
				current_key = key_press();
				switch (current_key) {
					case 1:
						page = UNLOCK_PAGE; // set to unlock page
						key_stay(); //wait key press
						Check_Code_Page();
						break;
					case 2:
						page = UNLOCK_CHANGE_CODE_PAGE;
						key_stay();
						Check_Code_Page();
						break;
					case 3: 
					case 4: 
					case 5: 
					case 6: 
					case 7: 
					case 8: 
					case 9:
						page = WARNING_PAGE;
						break;
				}
				break;
			case UNLOCK_PAGE: 
				current_key = key_press();
				//check type_status and current_key input to increase the check_typing and start typing current code
				if (check_typing < 6) {
					if (current_key > 0 && type_status) {
						typing();
						Check_Code_Page();
					}
				} 
				// check the value on array is sufficent 
				if (check_typing == 6) {
					Delay_s(2000000);// delay before changing to next page
					i=0;
					while(i<6){
						//compare the current input and default_code is different 
						if (default_code[i] != current_code[i]) {
							page = INCORRECT_PAGE;
							Incorrect_Page();
							break;
						}
						i++;
					}
					//when the current input matches with default_code then show correct title
					if (page != INCORRECT_PAGE) {
						Code_correct_Page();
						page = CORRECT_PAGE;
					}
					timer0_compare_value(1000);// the CORRECT PAGE or INCORRECT PAGE display in 1 second
					start_counting();
					reset_typing();
				}
				
				break;
			case UNLOCK_CHANGE_CODE_PAGE: 
				PA->DOUT |= (0b1 << 14);
				current_key = key_press();
			//check type_status and current_key input to increase the check_typing and start typing current code
				if (check_typing < 6) {
					if (current_key > 0 && type_status) {
						typing();
						Check_Code_Page();
					}
				} 
			// check the value on array is sufficent
				if (check_typing == 6) {
					Delay_s(2000000);// delay before changing to next page
					i=0;
					while(i<6){
						//compare the current input and default_code is different 
						if (default_code[i] != current_code[i]) {
							incorrect_code_count = incorrect_code_count + 1;//increase 1 per incorrect key code input
							page = CURRENT_CODE_INCORRECT_PAGE;
							Current_Code_Incorrect_Page();//display INCORRECT key code message and ask user try again until 3 times incorrect key code entered.
							break;
						}
						i++;
					}
					timer0_compare_value(1000);// the INCORRECT PAGE display in 1 second
					start_counting();
					reset_typing();
				//when the current input matches with default_code then change to CHANGE_CODE_PAGE
				if (page != CURRENT_CODE_INCORRECT_PAGE) {
						Can_Set_New_Code_Page();
						page = MOVE_PAGE;
						incorrect_code_count = 0;
						timer0_compare_value(2000);// the Move Page Message display in 2 second and move to change code page
						start_counting();
						reset_typing();
					}
					
				}
				
				break;
			case CHANGE_CODE_PAGE:
				//turn off the green LED				
				PA->DOUT |= (0b1 << 13);
				current_key = key_press();
				//check type_status and current_key input to increase the check_typing and start typing new code
				if (check_typing < 6) {
					if (current_key > 0 && type_status) {
						typing();
						Change_Page();
					}
				}
				//check enough 6 digits input then store new code to default code array
				if (check_typing == 6) {
					Delay_s(2000000);// delay before changing to next page
					store_new_code();
					Change_sucessful_Page();
					timer0_compare_value(1000);// the CHANGE CODE SUCCESSFUL PAGE display in 1 second
					start_counting();
					reset_typing();
					page = CORRECT_PAGE;
				}
				
				
				break;
			case WARNING_PAGE:
				current_key = key_press();
				if(current_key > 2){
					while(page==WARNING_PAGE){
					page=INCORRECT_PAGE;
					Warning_Page();
					}
				}
				timer0_compare_value(500);// the CORRECT PAGE or INCORRECT PAGE display in 500ms
				start_counting();
				break;
			case CURRENT_CODE_INCORRECT_PAGE:
			//If user enter incorrect key code 3 times at CHANGE CODE PAGE, the System will back to MENU PAGE
				while(incorrect_code_count==3){
					page = INCORRECT_PAGE;
					Three_Incorrect_Page();
					incorrect_code_count = 0;
					timer0_compare_value(2000);
				}
				break;
			default: break;	
		}
	};
}
//Button Interrupt
void EINT1_IRQHandler(void){
		while (!(PB->PIN & 1 << 15)){
			button_flag = 0;
			page = MENU_PAGE;
		
			//Turn off the RGB LED
			PA->DOUT |= (0b1 << 13);
			PA->DOUT |= (0b1 << 14);
			PA->DOUT |= (0b1 << 12);
			incorrect_code_count = 0;
			// reset the current code
			char reset_code[6] = {'1', '2', '3', '4', '5', '6'};
			for (i = 0; i< 6; i++) {
				default_code[i] = reset_code[i];
			}
			reset_typing();
			Menu_Page();
		}
		button_flag = 1;
		
		//EINT1 interrupt flag
		PB->ISRC |= (0b1 << 15);
}

//Timer Interrupt
void TMR0_IRQHandler(void){
		//when  INCORRECT , CORRECT and CODE CHANGED SUCCESSFULL PAGE are triggered , the LCD stay in 1 second before back to the MENU PAGE
		if (page == INCORRECT_PAGE || page == CORRECT_PAGE) {
			page = MENU_PAGE;
			Menu_Page();
		}
		//After the UNLOCK CHANGE CODE PAGE the LCD will move to the CHANGE CODE PAGE
		else if (page == MOVE_PAGE){
			page = CHANGE_CODE_PAGE;
			Change_Page();
			type_status = PRESS;
		}
		//After user enter the incorrect code in UNLOCK CHANGE CODE PAGE, the LCD will back to the enter current key code until ->
		//user enter the correct code.
		else if (page == CURRENT_CODE_INCORRECT_PAGE){
			page = UNLOCK_CHANGE_CODE_PAGE;
			Check_Code_Page();
			type_status = PRESS;
		}	
		//In the UNLOCK PAGE, CHANGE_CODE_PAGE and UNLOCK CHANGE CODE PAGE the code typing will be display in 200ms then become the '*'
		else if (page == UNLOCK_PAGE || page == UNLOCK_CHANGE_CODE_PAGE) {
			code_count[check_typing - 1] = '*';
			Check_Code_Page();//Display unlock lock page
			type_status = PRESS;
		}else if (page == CHANGE_CODE_PAGE) {
			code_count[check_typing - 1] = '*';
			Change_Page();//Display enter new 6 digits
			type_status = PRESS;
		}
		//Timer0 interrupt flag
		Clear_TIMER0();
}
