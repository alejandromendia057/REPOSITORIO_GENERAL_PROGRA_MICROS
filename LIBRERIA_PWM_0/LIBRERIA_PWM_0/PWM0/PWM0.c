/*
 * PWM0.c
 *
 * Created: 28/04/2026 10:29:52
 *  Author: Omar Cabrera 
 */ 

#include "PWM0.h"

void initPWM0A(uint8_t invertido, uint8_t modo, uint16_t prescaler)
{
	DDRD |= (1 << DDD6);   // OC0A en PD6

	TCCR0A = 0;
	TCCR0B = 0;

	if (invertido) {
		TCCR0A |= (1 << COM0A1) | (1 << COM0A0);
		} else {
		TCCR0A |= (1 << COM0A1);
	}

	if (modo == FAST_PWM) {
		TCCR0A |= (1 << WGM01) | (1 << WGM00);   // Fast PWM, TOP=255
	} else if (modo == PHASE_PWM_8BIT)
	{
		TCCR0A |= (1 << WGM00);
	} else if (modo == PHASE_PWM_TOP_OCR0A)
	{
		TCCR0A |= (1 << WGM00);
		TCCR0B |= (1 << WGM02);
	}

	switch(prescaler) {
		case 1:    TCCR0B |= (1 << CS00); break;
		case 8:    TCCR0B |= (1 << CS01); break;
		case 64:   TCCR0B |= (1 << CS01)|(1 << CS00); break;
		case 256:  TCCR0B |= (1 << CS02); break;
		case 1024: TCCR0B |= (1 << CS02)|(1 << CS00); break;
		default:   TCCR0B |= (1 << CS00); break;
	}
}

void initPWM0B(uint8_t invertido, uint8_t modo, uint16_t prescaler)
{
	DDRD |= (1 << DDD5);   // OC0B en PD5

	TCCR0A = 0;
	TCCR0B = 0;

	if (invertido) {
		TCCR0A |= (1 << COM0B1) | (1 << COM0B0);
		} else {
		TCCR0A |= (1 << COM0B1);
	}

	if (modo == FAST_PWM) {
		TCCR0A |= (1 << WGM01) | (1 << WGM00);   // Fast PWM, TOP=255
	} else if (modo == PHASE_PWM_8BIT)
	{
		TCCR0A |= (1 << WGM00);
	} else if (modo == PHASE_PWM_TOP_OCR0A)
	{
		TCCR0A |= (1 << WGM00);
		TCCR0B |= (1 << WGM02);
	}

	switch(prescaler) {
		case 1:    TCCR0B |= (1 << CS00); break;
		case 8:    TCCR0B |= (1 << CS01); break;
		case 64:   TCCR0B |= (1 << CS02); break;
		case 256:  TCCR0B |= (1 << CS02) | (1 << CS01); break;
		case 1024: TCCR0B |= (1 << CS02) | (1 << CS01) | (1 << CS00); break;
		default:   TCCR0B |= (1 << CS00); break;
	}
}

void initPWM0_Servo(uint16_t prescaler, uint8_t top)
{
	DDRD |= (1 << DDD5);  // SOLO EL PD5 (OC0B) QUEDA COMO SALIDA PARA CONTROLAR EL DUTY CYCLE DEL SERVO
	
	TCCR0A = 0;
	TCCR0B = 0;
	
	TCCR0A &= ~((1 << COM0A1)|(1 << COM0A0)); // CANAL A QUEDA DESCONECTADO, NO GENERA SALIDA EN PD6
	TCCR0A |= (1 << COM0B1); // CANAL B EN MODO NO INVERTIDO
	
	// MODO 5, PHASE CORRECT CON TOP EN OCRA
	TCCR0A |= (1 << WGM00);
	TCCR0B |= (1 << WGM02);
	
	OCR0A = top; // SE ESTABLECE EL TOP
	
	switch(prescaler){
		case 1: TCCR0B |= (1 << CS00); break;
		case 8: TCCR0B |= (1 << CS01); break;
		case 64: TCCR0B |= (1 << CS02); break;
		case 256: TCCR0B |= (1 << CS02)|(1 << CS01); break;
		case 1024: TCCR0B |= (1 << CS02)|(1 << CS01)|(1 << CS00); break;
		default: TCCR0B |= (1 << CS00); break;    // CASO DEFAULT, PRESCALER DE 1
	}
}
void updateDutyCycle0A(uint8_t ciclo)
{
	OCR0A = ciclo;
}

void updateDutyCycle0B(uint8_t ciclo)
{
	OCR0B = ciclo;
}
void updateServo0B(uint8_t ciclo)
{
	OCR0B = ciclo; 
}