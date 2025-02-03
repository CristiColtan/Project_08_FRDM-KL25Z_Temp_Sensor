#include "pit.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"

uint8_t led_state;
char temp_c='1';

void PIT_Init(void) {
	// Activarea semnalului de ceas pentru perifericul PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	// Utilizarea semnalului de ceas pentru tabloul de timere
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	// Oprirea decrementarii valorilor numaratoarelor in modul debug	
	PIT->MCR |= PIT_MCR_FRZ_MASK;

	// Setarea valoarea numaratorului de pe canalul 0 la o perioada de 1169 ms
	PIT->CHANNEL[0].LDVAL = 0x00B9CEBA;
	// Activarea intreruperilor pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	// Activarea timerului de pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	PIT->CHANNEL[1].LDVAL =  0x1E11F72;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	led_state=0;
	
	// Activarea intreruperii mascabile si setarea prioritatii
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,5);
	NVIC_EnableIRQ(PIT_IRQn);
}

void PIT_IRQHandler(void) {
	
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {

	// verificarea ordinii secventei si sincronizarea cu butonul din interfata	
	if(change_direction!=temp_c)	{
			OutputPIN_Init();
			temp_c=change_direction;
			led_state=4;
		}
	
		if(temp_c=='0')	{
						if(led_state == 0) {   //negru
				GPIOB_PCOR |= (1<<RED_LED_PIN);
				GPIOB_PCOR |= (1<<GREEN_LED_PIN); 
				GPIOD_PCOR |= (1<<BLUE_LED_PIN); 
				led_state = 1;
			} else if (led_state == 1) {  //turcoaz
				GPIOD_PSOR |= (1<<BLUE_LED_PIN); 
				GPIOB_PSOR |= (1<<GREEN_LED_PIN); 
				led_state = 2;
			} else if (led_state == 2) {  // rosu
				GPIOD_PCOR |= (1<<BLUE_LED_PIN); 
				GPIOB_PCOR |= (1<<GREEN_LED_PIN); 
				GPIOB_PSOR |= (1<<RED_LED_PIN);  
				led_state = 3;
			} 
			else if(led_state==3)   { //alb
				GPIOD_PSOR |= (1<<BLUE_LED_PIN); 
				GPIOB_PSOR |= (1<<GREEN_LED_PIN); 
				led_state = 0;
			}
			else if(led_state==4)  { //oprite toate
				GPIOB_PSOR |= (1<<RED_LED_PIN);  
				GPIOD_PSOR |= (1<<BLUE_LED_PIN); 
				GPIOB_PSOR |= (1<<GREEN_LED_PIN); 
				led_state=0;
			}
		}
		else	{
			if(led_state == 0) {  //alb
				GPIOB_PSOR |= (1<<RED_LED_PIN);  
				GPIOD_PSOR |= (1<<BLUE_LED_PIN); 
				GPIOB_PSOR |= (1<<GREEN_LED_PIN); 
				
				led_state = 1;
			} else if (led_state == 1) {  //rosu
				GPIOD_PCOR |= (1<<BLUE_LED_PIN); 
				GPIOB_PCOR |= (1<<GREEN_LED_PIN); 
				led_state = 2;
			} else if (led_state == 2) {   //turcoaz
				GPIOB_PCOR |= (1<<RED_LED_PIN); 
				GPIOD_PSOR |= (1<<BLUE_LED_PIN);  
				GPIOB_PSOR |= (1<<GREEN_LED_PIN);
				led_state = 3;
			} 
			else if(led_state==3)   { //nergu 
				GPIOD_PCOR |= (1<<BLUE_LED_PIN);  
				GPIOB_PCOR |= (1<<GREEN_LED_PIN);
				led_state = 0;
			}
			else if(led_state==4)  { //le am oprit pe toate
				GPIOB_PSOR |= (1<<RED_LED_PIN);
				GPIOD_PSOR |= (1<<BLUE_LED_PIN);  
				GPIOB_PSOR |= (1<<GREEN_LED_PIN);
				led_state=0;
			}
		}

		
		ADC0->SC1[0] |= ADC_SC1_AIEN_MASK;

		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
	}
	
	if(PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		
		Temperature_Read();
		playBuzzer();
		
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
	}
}