#include "gpio.h"

void OutputPIN_Init(void){
	
	//activarea ceasului pentru perifericul de port A
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	
	// --- BUZZER PIN --- 
	PORTA->PCR[BUZZER_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[BUZZER_PIN] |= PORT_PCR_MUX(1);

	// Configurare pin pe post de output
	GPIOA_PDDR |= (1<<BUZZER_PIN);
	// Oprirea buzzerului (punerea pe 1 logic -> active low)
	GPIOA_PCOR |= (1<<BUZZER_PIN);  //set e stins

	// --- RED LED ---
	
	// Utilizare GPIO ca varianta de multiplexare
	PORTB->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED_PIN] |= PORT_PCR_MUX(1);
	
	// Configurare pin pe post de output
	GPIOB_PDDR |= (1<<RED_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOB_PSOR |= (1<<RED_LED_PIN); //scriu 1 aprinde ledul
	
	// --- BLUE LED ---
	
	// Utilizare GPIO ca varianta de multiplexare
	PORTD->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_PIN] |= PORT_PCR_MUX(1);
	
	// Configurare pin pe post de output
	GPIOD_PDDR |= (1<<BLUE_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOD_PSOR |= (1<<BLUE_LED_PIN);  //set e stins

	// --- GREEN LED ---

	// Utilizare GPIO ca varianta de multiplexare
	PORTB->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_PIN] |= PORT_PCR_MUX(1);

	// Configurare pin pe post de output	
	GPIOB_PDDR |= (1<<GREEN_LED_PIN);

	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOB_PSOR |= (1<<GREEN_LED_PIN);  
}