/*
 * TIMER0_INTERRUPCION.c
 *
 * Created: 19/04/2026 21:29:37
 *  Author: aleja
 */ 

#include "TIMER0_INTERRUPCION.h"

void timer0_init(uint8_t modo, uint16_t prescaler, uint8_t ocr0a){
	
	// SE REINICIAN LOS REGISTROS DE CONTROL 
	
	TCCR0A = 0; 
	TCCR0B = 0; 
	TCNT0 =  0; 
	
	// CONFIGURACIÓN DEL MODO DE OPERACIÓN 
	if (modo == TIMER0_MODO_CTC){			
		TCCR0A |= (1 << WGM01);				// CTC MODE, TOP = OCR0A 
		OCR0A = ocr0a;						// CARGA VALOR DE COMPARACIÓN 
	}
	switch (prescaler){
		case 1: TCCR0B |= (1 << CS00); break; 
		case 8: TCCR0B |= (1 << CS01); break; 
		case 64: TCCR0B |= (1 << CS01)|(1 << CS00); break; 
		case 256: TCCR0B |= (1 << CS02); break; 
		case 1024: TCCR0B |= (1 << CS02)|(1 << CS00); break;
		default: 
		 TCCR0B |= (1 << CS00); break;  // PRESCALER DE 1 POR DEFAULT 
	}
}

void timer0_habilitar_interrupcion_compA(void){
	TIMSK0 |= (1 << OCIE0A);
}

void timer0_deshabilitar_interrupcion_compA(void){
	TIMSK0 &= ~(1 << OCIE0A);
}

void timer0_habilitar_interrupcion_ovf(void){
	TIMSK0 |= (1 << TOIE0); 
}

void timer0_deshabilitar_interrupcion_ovf(void){
	TIMSK0 &= ~(1 << TOIE0);
}

void timer0_reset_contador(void){
	TCNT0 = 0; 
}

uint8_t timer0_get_contador(void){
	return TCNT0; 
}