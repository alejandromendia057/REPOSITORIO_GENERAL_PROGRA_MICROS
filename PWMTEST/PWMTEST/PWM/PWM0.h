/*
 * PWM0.h
 *
 * Created: 14/04/2026 06:57:51
 *  Author: aleja
 */ 


#ifndef PWM0_H_
#define PWM0_H_

#include <avr/io.h>
#define invertir 1 
#define no_invertir 0

#define fastPWM 1
#define phasePWM 0 


void initPWM0A(uint8_t invertido, uint8_t modo, uint16_t prescaler);
void initPWM0B(uint8_t invertido, uint8_t modo, uint16_t prescaler);
void updateDutyCycle0A(uint8_t ciclo);
void updateDutyCycle0B(uint8_t ciclo);



#endif /* PWM0_H_ */