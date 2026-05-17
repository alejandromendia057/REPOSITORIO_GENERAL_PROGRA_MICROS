/*
 * ADC.c
 *
 * Created: 28/04/2026 11:40:00
 *  Author: Omar Cabrera 
 */ 

#include "ADC.h"

void initADC(uint8_t voltaje, uint8_t formato, uint8_t canal, uint8_t prescaler, uint8_t interrupcion)
{
	// INICIALIZACIÓN DE ADMUX
	ADMUX = 0;
	
	// INICIALIZACIÓN DE ADCSRA
	ADCSRA = 0;
	
	switch(voltaje)
	{
		// VOLTAJE APAGADO 
		case 0: 
		ADMUX &=~((1 << REFS1)|(1 << REFS0));
		break; 
		
		// VOLTAJE AVCC 
		case 1: 
		ADMUX |= (1 << REFS0); 
		break; 
		
		// RESERVADO 
		case 2: 
		ADMUX |= (1 << REFS1);
		break;  
		
		// 1.1V REFERENCIA INTERNA 
		case 3: 
		ADMUX |= (1 << REFS1)|(1 << REFS0); 
		break; 
		
		// DEFAULT VOLTAJE VCC 
		default: 
		ADMUX |= (1 << REFS0);
		break;
	}
	
	// FORMATO DE JUSTIFICACIÓN 
	if(formato == 1)
	{
		// JUSTIFICACIÓN A LA IZQUIERDA 
		ADMUX |= (1 << ADLAR); 
	}else
	{
		// JUSTIFICACIÓN A LA DERECHA 
		ADMUX &=~(1 << ADLAR); 
	}
	
	switch(canal)
	{
		// CANAL ADC0 NO SE HACE NADA PORQUE EL ADMUX EMPIEZA EN CEROS 
		case 0:
		break; 
		
		// CANAL ADC1 
		case 1: 
		ADMUX |= (1 << MUX0);
		break; 
		
		// CANAL ADC2 
		case 2:
		ADMUX |= (1 << MUX1);
		break; 
		
		// CANAL ADC3
		case 3:
		ADMUX |= (1 << MUX1)|(1 << MUX0);
		break; 
		
		// CANAL ADC4
		case 4:
		ADMUX |= (1 << MUX2);
		break; 
		
		// CANAL ADC5
		case 5:
		ADMUX |= (1 << MUX2)|(1 << MUX0);
		break; 
		
		// CANAL ADC6
		case 6:
		ADMUX |= (1 << MUX2)|(1 << MUX1);
		break; 
		
		// CANAL ADC7
		case 7:
		ADMUX |= (1 << MUX2)|(1 << MUX1)|(1 << MUX0);
		break; 
		
		// CANAL DEFAULT ADC0 
		default:
		break; 
	}
	
	switch(prescaler)
	{
		// FACTOR DE DIVISIÓN 2 
		case 2: 
		ADCSRA |=(1 << ADPS0); 
		break; 
		
		// FACTOR DE DIVISIÓN 4
		case 4:
		ADCSRA |=(1 << ADPS1);
		break;
		
		// FACTOR DE DIVISIÓN 8
		case 8:
		ADCSRA |=(1 << ADPS1)|(1 << ADPS0);
		break;
		
		// FACTOR DE DIVISIÓN 16
		case 16:
		ADCSRA |=(1 << ADPS2);
		break;
		
		// FACTOR DE DIVISIÓN 32
		case 32:
		ADCSRA |=(1 << ADPS2)|(1 << ADPS0);
		break;
		
		// FACTOR DE DIVISIÓN 64
		case 64:
		ADCSRA |=(1 << ADPS2)|(1 << ADPS1);
		break;
		
		// FACTOR DE DIVISIÓN 128
		case 128:
		ADCSRA |=(1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);
		break;
		
		// DEFAULT DIVISIÓN DE 128 SI EL RELOJ ESTÁ A 16 MHZ ES LA MEJOR OPCIÓN
		default: 
		ADCSRA |=(1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);
		break; 
	}
	
	if (interrupcion == 1)
	{
		// SE ACTIVAN LAS INTERRUPCIONES DE ADC 
		ADCSRA |= (1 << ADIE); 
	}else
	{
		ADCSRA &= ~(1 << ADIE); 
	}
	
	// SE HABILITA EL ADC 
	ADCSRA |= (1 << ADEN); 
}

void ADC_iniciar_conversion(void)
{
	ADCSRA |= (1 << ADSC);
}

void ADC_cambiar_canal(uint8_t canal)
{
	ADMUX &= ~((1 << MUX3)|(1 << MUX2)|(1 << MUX1)|(1 << MUX0)); 
	switch(canal)
	{
		// CANAL ADC0 NO SE HACE NADA PORQUE EL ADMUX EMPIEZA EN CEROS
		case 0:
		break;
		
		// CANAL ADC1
		case 1:
		ADMUX |= (1 << MUX0);
		break;
		
		// CANAL ADC2
		case 2:
		ADMUX |= (1 << MUX1);
		break;
		
		// CANAL ADC3
		case 3:
		ADMUX |= (1 << MUX1)|(1 << MUX0);
		break;
		
		// CANAL ADC4
		case 4:
		ADMUX |= (1 << MUX2);
		break;
		
		// CANAL ADC5
		case 5:
		ADMUX |= (1 << MUX2)|(1 << MUX0);
		break;
		
		// CANAL ADC6
		case 6:
		ADMUX |= (1 << MUX2)|(1 << MUX1);
		break;
		
		// CANAL ADC7
		case 7:
		ADMUX |= (1 << MUX2)|(1 << MUX1)|(1 << MUX0);
		break;
		
		// CANAL DEFAULT ADC0
		default:
		break;
	}
}

uint16_t ADC_leer(uint8_t canal)
{
	ADC_cambiar_canal(canal);
	ADC_iniciar_conversion(); 
	while (ADCSRA & (1 << ADSC)); // ESPERAR A QUE ADSC SE HAGA 0 
	return ADC; 
}