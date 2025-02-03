#include "uart.h"

volatile char change_direction='0';

// Functie pentru inversarea bitilor (MSB first)
uint8_t reverse_bits(uint8_t data) {
    uint8_t reversed = 0;
    for (int i = 0; i < 8; i++) {
        reversed = (reversed << 1) | (data & 1);
        data >>= 1;
    }
    return reversed;
}


void UART0_IRQHandler(void)
{
	if(UART0->S1 & UART0_S1_RDRF_MASK)	{
		change_direction=reverse_bits(UART0->D);
	}
}

void UART0_TransmitMessage(const char* message) {
    while (* message) { 
        UART0_Transmit(* message++);
    }       
} 

void UART0_Transmit(uint8_t data)
{
    uint8_t reversed_data = reverse_bits(data);  // Inversam bitii pentru MSB first
    while (!(UART0->S1 & UART0_S1_TDRE_MASK));   // Asteptam sa fie liber bufferul de transmisie
    UART0->D = reversed_data;                    // Transmitem datele inversate
}

uint8_t UART0_Receive(void)
{
    while (!(UART0->S1 & UART0_S1_RDRF_MASK));   // Asteptam sa fie primite date
    uint8_t received_data = UART0->D;            // Citim datele primite
    return reverse_bits(received_data);          // Inversam bitii pentru a obtine ordinea corecta MSB first
}


void UART0_Initialize(uint32_t baud_rate) {
	
	//Setarea sursei de ceas pt modulul UART
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(01);
	
	//Activarea semnalului de ceas pt modulul UART
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	
	//Activarea semnalului de ceas pentru portul A
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	PORTA->PCR[1] = ~PORT_PCR_MUX_MASK; //Resetare PTA1
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); 
	//Configurare RX pt modulul UART si curatare ISF
	PORTA->PCR[2] = ~PORT_PCR_MUX_MASK; //Resetare PTA2
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); 
	//Configurare TX pt modulul UART si curatare ISF
	
	//Dezactivarea temporara RX, TX pt modulul UART 
	UART0->C2 &= ~((UART0_C2_RE_MASK) | (UART0_C2_TE_MASK)); 

	uint16_t osr = 15; //Over-Sampling Rate
	uint16_t sbr; //Serial Baud Rate
	
	//sbr = (uint16_t)((DEFAULT_SYSTEM_CLOCK)/(baud_rate * (osr))); //System clock 48MHz/2 
	//UART0->BDH &= UART0_BDH_SBR_MASK; //0x1F00
	//UART0->BDH |= UART0_BDH_SBR(sbr>>8); //Higher bits of Baud Rate
	//UART0->BDL = UART_BDL_SBR(sbr); //Lower bits of Baud Rate
	//UART0->C4 |= UART0_C4_OSR(osr - 1); //setare Over-Sampling Rate
	
	sbr = (uint16_t)((DEFAULT_SYSTEM_CLOCK) / (baud_rate * (osr + 1)));
	UART0->BDH = UART0_BDH_SBR((uint8_t)((sbr&0x1F00)>>8));
	UART0->BDL = (uint8_t)(sbr&0xFF);
	UART0->C4 = UART0_C4_OSR_MASK & UART0_C4_OSR(osr);
	
//SBR 	 - vom retine valoarea baud rate-ului calculat pe baza frecventei ceasului de sistem
//SBR  	 -	   b16 b15 b14 [b13   b12 b11 b10 b09		 b08 b07 b06 b05   b04 b03 b02 b01] &
//0x1F00 - 	  	0		0   0    1     1   1   1   1      0   0   0   0     0   0   0   0
//              0   0   0   b13   b12 b11 b10 b09     0   0   0   0     0   0   0   0 >> 8
//BDH    -      0   0   0   b13   b12 b11 b10 b09
//BDL    -     b08 b07 b06  b05   b04 b03 b02 b01
	
	//Setarea numarului de biti de date la 8 si fara bit de paritate
	//UART0->C1 = UART0_C1_M(0) | UART0_C1_PE(0);
	UART0->C1=0;
	
	//Transmisie incepand cu MSB
	UART0->S2 = UART0_S2_MSBF(1);
	
	//Dezactivare intreruperi la transmisie
	UART0->C2 |= UART0_C2_TIE(0);
	UART0->C2 |= UART0_C2_TCIE(0);
	
	//Activare intreruperi la receptie
	UART0->C2 |= UART0_C2_RIE(1);
	
	//Activare RX, TX pt modulul UART
	UART0->C2 |= ((UART_C2_RE_MASK) | (UART_C2_TE_MASK));
		
	NVIC_EnableIRQ(UART0_IRQn);
}
