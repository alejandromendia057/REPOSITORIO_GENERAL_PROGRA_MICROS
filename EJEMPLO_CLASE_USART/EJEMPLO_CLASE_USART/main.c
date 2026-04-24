/*
 * EJEMPLO_CLASE_USART.c
 *
 * Created: 16/04/2026 15:21:13
 * Author : aleja
 */ 

/****************************************/
// Encabezado (Libraries)

#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART/UART.h"
/****************************************/
// Function prototypes

void initUART();
void writeChar(char c);
void writeString(char* string);


/****************************************/
// Main Function

int main(void)
{
	cli();
	DDRB |= (1 << DDB5);
	PORTB &= ~(1 << PORTB5); 
	DDRD |= (1 << DDD5);
	PORTD &= ~(1 << PORTD5);
	initUART();
	writeChar("Hola sección 10");
	sei();
	while (1)
	{
		
	}
}

/****************************************/
// NON-Interrupt subroutines


/****************************************/
// Interrupt routines
ISR(USART_RX_vect)
{
	uint8_t bufferRX = UDR0; 
	writeChar(bufferRX);
	if (bufferRX == 'a')
	{
		PORTB|= (1 << PORTB5);
		PORTD|= (1 << PORTD5); 
	}
	if (bufferRX == 'b')
	{
		PORTB &= ~(1 << PORTB5);
		PORTD &= ~(1 << PORTD5);
	}
}