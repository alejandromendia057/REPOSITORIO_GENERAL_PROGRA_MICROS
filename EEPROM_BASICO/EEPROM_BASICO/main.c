/*
 * EEPROMbasico.c
 *
 * Created: 23/04/2026 14:41:18
 * Author : Omar Cabrera 
 */ 

/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART/UART.h"
#include "EEPROM_LIB/eeprom.h"

/****************************************/
// Function prototypes
uint16_t	dir_eeprom = 0;
const char* L1_on	= "L1:1";
const char* L1_off	= "L1:0";
const char* L2_on	= "L2:1";
const char* L2_off	= "L2:0";

char* string_rec	= "----";
uint8_t num_cara	= 0;

void estadoLEDs();

/****************************************/
// Main Function
int main ()
{
	cli();
	DDRD	|= ((1<<DDD6) | (1<<DDD5));  // PD5 y PD6 como salidas (LEDs) 
	PORTD	&= ~((1<<DDD6) | (1<<DDD5)); // PD5 y PD6 inicialmente apagados
	
	DDRD	&= ~(1<<DDD2);				 // PD2 como entrada
	PORTD	|= (1<<PORTD2);				 // PD2 con pull-up activado
	
	PCICR	|= (1<<PCIE2);				 // Habilitamos interrupciones pin change para port D
	PCMSK2	|= (1<<PCINT18); 
	
	estadoLEDs();
	initUART();
	sei();
	/*uint8_t lectura = readEEPROM(dir_eeprom);
	while(lectura != 0xFF) // Si no esta vacío lo mandamos por serial 
	{
		writeChar(lectura);
		//writeEEPROM(dir_eeprom, 0xFF); // Si se quisiera borrar lo que esta en EEPROM hacemos lineas comentadas
		dir_eeprom++;
		lectura = readEEPROM(dir_eeprom);
	}
	//dir_eeprom = 0;*/
	while(1)
	{
	}
}
/****************************************/
// NON-Interrupt subroutines
void estadoLEDs()
{
	uint8_t estadoL1 = readEEPROM(0);
	uint8_t estadoL2 = readEEPROM(1);
	if (estadoL1 == 1)
	{
		PORTD	|= (1<<PORTD5);
	}
	if (estadoL2 == 1)
	{
		PORTD	|= (1<<PORTD6);
	}
}
/****************************************/
// Interrupt routines
ISR(USART_RX_vect)
{
	uint8_t rx_buffer = UDR0;
	if(rx_buffer != '\n')
	{
		writeChar(rx_buffer);
		*(string_rec + num_cara) = rx_buffer;
		num_cara++;
		if (*(string_rec+0) == *(L1_on+0) &&
			*(string_rec+1) == *(L1_on+1) &&
			*(string_rec+2) == *(L1_on+2))
		{
			if (*(string_rec+3) == *(L1_on+3))
			{
				PORTD	|= (1<<PORTD5);
				writeEEPROM(0, 1);
			}
			if (*(string_rec+3) == *(L1_off+3))
			{
				PORTD	|= ~(1<<PORTD5);
				writeEEPROM(0, 0);
			}
		}
		
		if (*(string_rec+0) == *(L2_on+0) &&
			*(string_rec+1) == *(L2_on+1) &&
			*(string_rec+2) == *(L2_on+2))
		{
			if (*(string_rec+3) == *(L2_on+3))
			{
				PORTD	|= (1<<PORTD6);
				writeEEPROM(1, 1);
			}
			if (*(string_rec+3) == *(L2_off+3))
			{
				PORTD	|= ~(1<<PORTD6);
				writeEEPROM(1, 0);
			}
		}
		/*if (rx_buffer == 'a')
		{
			PORTD ^= (1<<PORTD5)
		}
		if (rx_buffer == 'b')
		{
			PORTD ^= (1<<PORTD6)
		}*/
	}else{
		for (uint8_t i = 0; i<4; i++)
		{
			*(string_rec+i) = '-';
		}
		num_cara = 0;
		writeChar('\n');
	}
	
	writeChar(rx_buffer);
	//writeEEPROM(dir_eeprom, rx_buffer);
	//dir_eeprom++;
}

ISR(PCINT2_vect)
{
	uint8_t estadoPIND = PIND & (1<<PIND2);
	if(!(estadoPIND == (1<<PIND2)))
	{
		PORTD	^= (1<<PORTD5);
	}
}