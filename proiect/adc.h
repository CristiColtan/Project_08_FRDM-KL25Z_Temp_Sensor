#include "MKL25Z4.h"

void ADC0_Init(void);
int ADC0_Calibrate(void);
uint16_t ADC0_Read(uint8_t);
void ADC0_IRQHandler(void);
void Temperature_Read(void);
void Voltage_Read(void);
void playBuzzer(void);