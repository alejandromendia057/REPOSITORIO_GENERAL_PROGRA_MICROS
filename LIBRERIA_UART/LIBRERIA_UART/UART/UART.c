/*
 * UART.c
 *
 * Created: 28/04/2026 11:30:47
 *  Author: Omar Cabrera 
 */ 

#include "UART.h"

initUART()
{
	// CONFIGUAR PINES
	DDRD &= ~(1 << DDD0); // D0 RX ENTRADA
	DDRD |= (1 << DDD1); // D1 TX SALIDA
	// NORMAL SPEED
	UCSR0A = 0;
	
	// HABILITAR INTERRUPCIÓN RX; HABILITAR RX Y TX
	UCSR0B = (1 << RXCIE0)|(1 << RXEN0)|(1 << TXEN0);
	
	// MODO ASÍNCRONO, SIN PARIDAD, 1 STOP BIT, 8 DATA BITS
	UCSR0C = (1 << UCSZ01)|(1 << UCSZ00);
	
	// CARGAR UBRR0
	UBRR0 = 103;
}
void writeChar(char c)
{
	while(!((UCSR0A) & (1 << UDRE0)));
	UDR0 = c;
}
void writeString(char* string)
{
	for(uint8_t i=0; *(string + i)!= '\0'; i++)
	{
		writeChar(string[i]);
	}
}
