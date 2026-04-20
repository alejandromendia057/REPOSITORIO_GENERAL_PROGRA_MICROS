/*
 * PWM1.c
 *
 * Created: 14/04/2026 08:11:58
 *  Author: aleja
 */ 

#include "PWM1.h"

// FUNCIÓN AUXILIAR PARA ESCRIBIR UN VALOR DE 16 BITS EN UN REGISTRO, PRIMERO EL BYTE ALTO, LUEGO EL BAJO 

static void Registro_16bits(volatile uint16_t *reg, uint16_t valor){
	*((volatile uint8_t*) reg + 1) = (valor >> 8); 
	*((volatile uint8_t*) reg) = valor & 0xFF; 
}

void initPWM1A(uint8_t invertido, uint8_t modo, uint16_t prescaler, uint16_t top){
	
	// SE CONFIGURA COMO SALIDA EL PB1 (OC1A)
	DDRB |= (1 << DDB1); 
	
	// SE LIMPIAN LOS REGISTROS 
	
	TCCR1A = 0; 
	TCCR1B = 0;
	
	// CONFIGURACIÓN DE MODO INVERTIDO/ NO INVERTIDO EN 0C1A 
	if (invertido){
		
		TCCR1A |= (1 << COM1A1)|(1 << COM1A0); // MODO INVERTIDO PB1 
	}else{
		TCCR1A |= (1 << COM1A1); // MODO NO INVERTIDO 
	}
	
	// CONFIGURACIÓN DEL MODO PWM 
	
	switch(modo){
		
		// MODO 14: FAST PWM CON TOP = ICR1
		case MODO_FAST_PWM_ICR: 
			TCCR1A |= (1 << WGM11);
			TCCR1B |= (1 << WGM13)|(1 << WGM12);
			Registro_16bits((volatile uint16_t*)& ICR1, top);
			break; 
		
		// MODO 15: FAST PWM, TOP = OCR1A 
		case MODO_FAST_PWM_OCR: 
			TCCR1A |= (1 << WGM11)|(1 << WGM10);
			TCCR1B |= (1 << WGM13)|(1 << WGM12);
			Registro_16bits((volatile uint16_t*)& OCR1A, top);
			break; 
			
		// MODO 8: PHASE CORRECT PWM, TOP = ICR1
		case MODO_PHASE_CORRECT: 
			TCCR1A |= (1 << WGM11);
			TCCR1B |= (1 << WGM13);
			Registro_16bits((volatile uint16_t*)& ICR1, top);
			break; 
		
		// MODO DEFAULT: 14 
		
		default: 
		TCCR1A |= (1 << WGM11);
		TCCR1B |= (1 << WGM13)|(1 << WGM12);
		Registro_16bits((volatile uint16_t*)& ICR1, top);
		break;
	}
	
	// CONFIGURACIÓN DEL PRESCALER (BITS CS12, CS11, CS10)
	
	switch (prescaler){
		
		case 1: 
			TCCR1B |= (1 << CS10);
			break;
		case 8: 
			TCCR1B |= (1 << CS11);
			break; 
		case 64:
			TCCR1B |= (1 << CS11)|(1 << CS10);
			break; 
		case 256: 
			TCCR1B |= (1 << CS12);
			break; 
		case 1024: 
			TCCR1B |= (1 << CS12)|(1 << CS10);
			break; 
		default: 
			// PRESCALER DE 1 POR DEFECTO 
			TCCR1B |= (1 << CS10);
			break;
	}
}

void initPWM1B(uint8_t invertido, uint8_t modo, uint16_t prescaler, uint16_t top){
	
	// SE CONFIGURA COMO SALIDA EL PB2 (OC1B)
	DDRB |= (1 << DDB2);
	
	// SE LIMPIAN LOS REGISTROS
	
	TCCR1A = 0;
	TCCR1B = 0;
	
	// CONFIGURACIÓN DE MODO INVERTIDO/ NO INVERTIDO EN 0C1A
	if (invertido){
		
		TCCR1A |= (1 << COM1B1)|(1 << COM1B0); // MODO INVERTIDO PB2
		}else{
		TCCR1A |= (1 << COM1B1); // MODO NO INVERTIDO
	}
	
	// CONFIGURACIÓN DEL MODO PWM
	
	switch(modo){
		
		// MODO 14: FAST PWM CON TOP = ICR1
		case MODO_FAST_PWM_ICR:
		TCCR1A |= (1 << WGM11);
		TCCR1B |= (1 << WGM13)|(1 << WGM12);
		Registro_16bits((volatile uint16_t*)& ICR1, top);
		break;
		
		// MODO 15: FAST PWM, TOP = OCR1A
		case MODO_FAST_PWM_OCR:
		TCCR1A |= (1 << WGM11)|(1 << WGM10);
		TCCR1B |= (1 << WGM13)|(1 << WGM12);
		Registro_16bits((volatile uint16_t*)& OCR1A, top);
		break;
		
		// MODO 8: PHASE CORRECT PWM, TOP = ICR1
		case MODO_PHASE_CORRECT:
		TCCR1A |= (1 << WGM11);
		TCCR1B |= (1 << WGM13);
		Registro_16bits((volatile uint16_t*)& ICR1, top);
		break;
		
		// MODO DEFAULT: 14
		
		default:
		TCCR1A |= (1 << WGM11);
		TCCR1B |= (1 << WGM13)|(1 << WGM12);
		Registro_16bits((volatile uint16_t*)& ICR1, top);
		break;
	}
	
	// CONFIGURACIÓN DEL PRESCALER (BITS CS12, CS11, CS10)
	
	switch (prescaler){
		
		case 1:
		TCCR1B |= (1 << CS10);
		break;
		case 8:
		TCCR1B |= (1 << CS11);
		break;
		case 64:
		TCCR1B |= (1 << CS11)|(1 << CS10);
		break;
		case 256:
		TCCR1B |= (1 << CS12);
		break;
		case 1024:
		TCCR1B |= (1 << CS12)|(1 << CS10);
		break;
		default:
		// PRESCALER DE 1 POR DEFECTO
		TCCR1B |= (1 << CS10);
		break;
	}		
}

void updateDutyCycle1A(uint16_t valor)
{
	Registro_16bits((volatile uint16_t*)&OCR1A, valor);
}

void updateDutyCycle1B(uint16_t valor)
{
	Registro_16bits((volatile uint16_t*)&OCR1B, valor);
}

void setTop1(uint16_t top)
{
	Registro_16bits((volatile uint16_t*)&ICR1, top);
}