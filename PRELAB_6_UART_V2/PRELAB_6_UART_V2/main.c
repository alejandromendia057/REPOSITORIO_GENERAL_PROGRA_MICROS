/*
 * 
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
void displayASCII(uint8_t valor); 

/****************************************/
// Main Function

int main(void)
{
	cli();
	
	// TODOS LOS PINES DE LOS LEDS COMO SALIDA 	
	DDRD |= (1 << DDD2);
	PORTD &= ~(1 << PORTD2);
	DDRD |= (1 << DDD3);
	PORTD &= ~(1 << PORTD3);
	DDRD |= (1 << DDD4);
	PORTD &= ~(1 << PORTD4);
	DDRD |= (1 << DDD5);
	PORTD &= ~(1 << PORTD5);
	DDRD |= (1 << DDD6);
	PORTD &= ~(1 << PORTD6);
	DDRD |= (1 << DDD7);
	PORTD &= ~(1 << PORTD7);
	DDRB |= (1 << DDB0);
	PORTB &= ~(1 << PORTB0);
	DDRB |= (1 << DDB1);
	PORTB &= ~(1 << PORTB1);
	 
	initUART();
	sei();
	writeString("Hola Alma, como te va en tu proyecto?");
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
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;  
}
void writeString(char* string)
{
	for(uint8_t i=0; *(string + i) != '\0'; i++)
	{
		writeChar(*(string + i)); 
	}
}
void displayASCII(uint8_t valor)
{
	PORTD = (PORTD & 0x03)|((valor & 0x3F) << 2);
	PORTB = (PORTB & 0xFC)|((valor >> 6) & 0x03); 
}
/****************************************/
// Interrupt routines
ISR(USART_RX_vect)
{
	uint8_t bufferRX = UDR0; 
	writeChar(bufferRX);
	displayASCII(bufferRX);
}
