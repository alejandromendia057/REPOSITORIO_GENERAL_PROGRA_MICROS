/*
 * PWM0.c
 *
 * Created: 14/04/2026 06:58:57
 *  Author: aleja
 */ 
#include "PWM0.h"

void initPWM0A(uint8_t invertido, uint8_t modo, uint16_t prescaler)
{
	// CONFIGURACIÓN DE LAS SALIDAS
	DDRD |= (1 << DDD6);
	
	
	TCCR0A &= ~((1 << COM0A1)|(1 << COM0A0));
	TCCR0B = 0;
	
	if (invertido)
	{
		TCCR0A |= (1 << COM0A1)|(1 << COM0A0); // INVERTIDO PD6
	}else{
		TCCR0A |= (1 << COM0A1); // NO INVERTIDO PD6
	}
	
	if(modo)
	{
		TCCR0A |= (1 << WGM01)|(1 << WGM00); // FAST MODE
	}else{
		TCCR0A |= (1 << WGM00); // PHASE MODE
	}
	
	
	switch(prescaler)
	{
		case 1:
			TCCR0B |= (1 << CS00); 
			break; 
		case 8:
			TCCR0B |= (1 << CS01); 
			break; 
		case 64:
			TCCR0B |= (1 << CS01)|(1 << CS00);
			break; 
		case 256: 
			TCCR0B |= (1 << CS02); 
			break; 
		case 1024:
			TCCR0B |= (1 << CS02)|(1 << CS00); 
			break; 
		default:
			TCCR0B |= (1 << CS00);
			break; 
	}

}

void initPWM0B(uint8_t invertido, uint8_t modo, uint16_t prescaler)
{
	// CONFIGURACIÓN DE LAS SALIDAS
	DDRD |= (1 << DDD5);
	
	
	TCCR0A &= ~((1 << COM0B1)|(1 << COM0B0));
	TCCR0B = 0;
	
	if (invertido)
	{
		TCCR0A |= (1 << COM0B1)|(1 << COM0B0); // INVERTIDO PD6
		}else{
		TCCR0A |= (1 << COM0B1); // NO INVERTIDO PD6
	}
	
	if(modo)
	{
		TCCR0A |= (1 << WGM01)|(1 << WGM00); // FAST MODE
		}else{
		TCCR0A |= (1 << WGM00); // PHASE MODE
	}
	
	
	switch(prescaler)
	{
		case 1:
		TCCR0B |= (1 << CS00);
		break;
		case 8:
		TCCR0B |= (1 << CS01);
		break;
		case 64:
		TCCR0B |= (1 << CS01)|(1 << CS00);
		break;
		case 256:
		TCCR0B |= (1 << CS02);
		break;
		case 1024:
		TCCR0B |= (1 << CS02)|(1 << CS00);
		break;
		default:
		TCCR0B |= (1 << CS00);
		break;
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