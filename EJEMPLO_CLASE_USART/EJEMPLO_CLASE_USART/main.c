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
	while(!((UCSR0A) & (1 << UDRE0)))
	UDR0 = c; 
}
void writeString(char* string)
{
	for(uint8_t i=0; *(string + i)!= '\0'; i++)
	{
		writeChar(string[i]); 
	}
}
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