#include "MKL25Z4.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"
#include "pit.h"

int main() {
	
	UART0_Initialize(14400);
	OutputPIN_Init();
	ADC0_Init();
	PIT_Init();
	while(1){}
}