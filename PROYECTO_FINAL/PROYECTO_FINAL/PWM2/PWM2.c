/*
 * PWM2.c
 *
 * Created: 28/04/2026 10:25:12
 *  Author: Omar Cabrera 
 */ 


#include "PWM2.h"

void initPWM2A(uint8_t invertido, uint8_t modo, uint16_t prescaler)
{
	DDRB |= (1 << DDB3);   // OC2A en PB3

	TCCR2A = 0;
	TCCR2B = 0;

	if (invertido) {
		TCCR2A |= (1 << COM2A1) | (1 << COM2A0);
		} else {
		TCCR2A |= (1 << COM2A1);
	}

	if (modo == FAST_PWM) {
		TCCR2A |= (1 << WGM21) | (1 << WGM20);   // Fast PWM, TOP=255
	} else if (modo == PHASE_PWM_8BIT)
	{
		TCCR2A |= (1 << WGM20);
	} else if (modo == PHASE_PWM_TOP_OCR2A)
	{
		TCCR2A |= (1 << WGM20);
		TCCR2B |= (1 << WGM22);
	}

	switch(prescaler) {
		case 1:    TCCR2B |= (1 << CS20); break;
		case 8:    TCCR2B |= (1 << CS21); break;
		case 32:   TCCR2B |= (1 << CS21) | (1 << CS20); break;
		case 64:   TCCR2B |= (1 << CS22); break;
		case 128:  TCCR2B |= (1 << CS22) | (1 << CS20); break;
		case 256:  TCCR2B |= (1 << CS22) | (1 << CS21); break;
		case 1024: TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); break;
		default:   TCCR2B |= (1 << CS20); break;
	}
}

void initPWM2B(uint8_t invertido, uint8_t modo, uint16_t prescaler)
{
	DDRD |= (1 << DDD3);   // OC2B en PD3

	TCCR2A = 0;
	TCCR2B = 0;

	if (invertido) {
		TCCR2A |= (1 << COM2B1) | (1 << COM2B0);
		} else {
		TCCR2A |= (1 << COM2B1);
	}

	if (modo == FAST_PWM) {
		TCCR2A |= (1 << WGM21) | (1 << WGM20);   // Fast PWM, TOP=255
	} else if (modo == PHASE_PWM_8BIT)
	{
		TCCR2A |= (1 << WGM20);
	} else if (modo == PHASE_PWM_TOP_OCR2A)
	{
		TCCR2A |= (1 << WGM20);
		TCCR2B |= (1 << WGM22);
	}

	switch(prescaler) {
		case 1:    TCCR2B |= (1 << CS20); break;
		case 8:    TCCR2B |= (1 << CS21); break;
		case 32:   TCCR2B |= (1 << CS21) | (1 << CS20); break;
		case 64:   TCCR2B |= (1 << CS22); break;
		case 128:  TCCR2B |= (1 << CS22) | (1 << CS20); break;
		case 256:  TCCR2B |= (1 << CS22) | (1 << CS21); break;
		case 1024: TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); break;
		default:   TCCR2B |= (1 << CS20); break;
	}
}

void initPWM2_Servo(uint16_t prescaler, uint8_t top)
{
	DDRD |= (1 << DDD3);  // SOLO EL PD3 (OC2B) QUEDA COMO SALIDA PARA CONTROLAR EL DUTY CYCLE DEL SERVO
	
	TCCR2A = 0;
	TCCR2B = 0;
	
	TCCR2A &= ~((1 << COM2A1)|(1 << COM2A0)); // CANAL A QUEDA DESCONECTADO, NO GENERA SALIDA EN PB3
	TCCR2A |= (1 << COM2B1); // CANAL B EN MODO NO INVERTIDO
	
	// MODO 5, PHASE CORRECT CON TOP EN OCRA
	TCCR2A |= (1 << WGM20);
	TCCR2B |= (1 << WGM22);
	
	OCR2A = top; // SE ESTABLECE EL TOP
	
	switch(prescaler){
		case 1: TCCR2B |= (1 << CS20); break;
		case 8: TCCR2B |= (1 << CS21); break;
		case 32: TCCR2B |= (1 << CS21)| (1 << CS20); break;
		case 64: TCCR2B |= (1 << CS22); break;
		case 128: TCCR2B |= (1 << CS22)| (1 << CS20); break;
		case 256: TCCR2B |= (1 << CS22)|(1 << CS21); break;
		case 1024: TCCR2B |= (1 << CS22)|(1 << CS21)|(1 << CS20); break;
		default: TCCR2B |= (1 << CS20); break;    // CASO DEFAULT, PRESCALER DE 1
	}
}
void updateDutyCycle2A(uint8_t ciclo)
{
	OCR2A = ciclo;
}

void updateDutyCycle2B(uint8_t ciclo)
{
	OCR2B = ciclo;
}
void updateServo2B(uint8_t ciclo)
{
	OCR2B = ciclo;
}