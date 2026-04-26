/*
 * LABORATORIO_6_UART.c
 *
 * Created: 21/04/2026 19:16:37
 * Author : Omar Cabrera 
 */ 


/****************************************/
// Encabezado (Libraries)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>								// LIBRERÍA NECESARIA PARA CONVERTIR DE ENTERO A STRING 
#include "UART/UART.h"
/****************************************/
// Function prototypes

void displayASCII(uint8_t valor);				// FUNCIÓN QUE RECIBE EL VALOR ASCII DE UN CARACTER Y ENCIENDE LOS LEDS CORRESPONDIENTES A SU REPRESENTACIÓN BINARIA 
void initADC(void);								// FUNCIÓN PARA INICIALIZAR EL ADC 
void menu(void);  
// Variables

volatile uint8_t estado_funcion = 0;			// ESTA VARIABLE LE INDICARÁ A LA ISR EN QUE MODO ESTA LA "MÁQUINA" ACTUALMENTE, SI EL USUARIO ESTÁ EN MENÚ = 0, = 1 LEER EL POTENCIÓMETRO, = 2 ENVIAR ASCII
volatile uint16_t valor_ADC5 = 0;				// ALMACENA LOS 10 BITS DEL ADC5
volatile uint16_t valor_voltaje = 0;			// VOLTAJE LEÍDO DEL POTENCIÓMETRO
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
	initADC();	
	sei();
	menu();
	while (1)
	{
		
	}
}

/****************************************/
// NON-Interrupt subroutines

void displayASCII(uint8_t valor)
{
	// PORTD CONTIENE DEL BIT 0 AL 5, MÁSCARA CON 0b00000011 PARA DEJAR PD0 Y PD1 INTACTOS
	// VALOR (QUE CONTIENE UDRN0) & 0b00111111 PARA TRABAJAR SOLO LOS PRIMEROS 6 BITS, SE CORREN LOS BITS 2 VECES A LA IZQUIERDA PARA NO ALTERAR PD0 Y PD1
	// FINALMENTE UN OR PARA QUE PORTD ENCIENDA LOS PRIMEROS 6 BITS QUE CORRESPONDAN 
	PORTD = (PORTD & 0x03)|((valor & 0x3F) << 2);
	// PORTB CONTIENE LOS 2 BITS MÁS SIGNIFICATIVOS 
	// MÁSCARA CON 0b11111100 PARA NO ALTERAR LOS VALORES QUE TENGAN LOS PINES MÁS ALTOS DE PORTB 
	// EL VALOR DE UDRN0 SE CORRE 6 VECES A LA DERECHA PARA QUE EL OR SOLO ALTERE LOS 2 PINES DE B CON LO QUE SE TRABAJA 
	// MÁSCARA CON 0b00000011 POR MÉTODO DE SEGURIDAD 		
	PORTB = (PORTB & 0xFC)|((valor >> 6) & 0x03);
}

void initADC(void)
{
	
	// INICIALIZACIÓN DE ADMUX Y DE ADCSRA 
	ADMUX = 0;
	ADCSRA = 0; 
	
	// VOLTAJE DE REFERENCIA, JUSTIFICACIÓN A LA DERECHA, ADC5 ACTIVADO
	ADMUX |= (1 << REFS0)|(1 << MUX2)|(1 << MUX0);
		
	// CONFIGRUACIÓN DEL REGISTRO ADCSRA: ADEN PARA ACTIVAR EL ADC, ADPS2,1,0 PARA TENER 125 KHZ, ADATE =1 AUTO TRIGGER (CONVERSIÓN CONTINUA)
	ADCSRA = (1 << ADEN) | (1 << ADATE)|(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	// HABILITACIÓN DE INTERRUPCIONES E INICIAR LA PRIMERA CONVERSIÓN 
	ADCSRA |= (1 << ADIE)|(1 << ADSC); 
}

void menu(void)
{
	writeString("Bienvenido, presione 1 para leer el voltaje del potenciómetro, presione 2 para enviar un ASCII"); 
	
}
/****************************************/
// Interrupt routines
ISR(USART_RX_vect)
{
	// SE ALMACENA EN LA VARIABLE BUFFERRX LO QUE SEA QUE TENGA EL REGISTRO UDRN0 
	uint8_t bufferRX = UDR0;
	
	// MÉTODO DEFENSIVO POR SI SE UTILIZA OTRA TERMINAL, SI SE ENVÍAN LOS CARACTERES DE CONTROL '\r' o '\n' SE SALE DE LA ISR INMEDIATAMENTE 
	if (bufferRX == '\r' || bufferRX == '\n') return;

	// SI SE ESTÁ EN EL MENÚ (ESTADO_FUNCION == 0), SE ESPERA LA RESPUESTA DEL USUARIO 
	if (estado_funcion == 0)          
	{
		// SI SE RECIBE EN UDRN0 = '1' , EL USUARIO QUIERE LEER EL VOLTAJE DADO POR EL POTENCIÓMETRO
		if (bufferRX == '1')
		{
			// COMO LA CONVERSIÓN DEL ADC A VOLTAJE PUEDE DAR DECIMALES, ES NECESARIO SEPARAR LA PARTE ENTERA DE LA DECIMAL 
			
			// ENTERO ALMACENA EL RESULTADO DE LA DIVISIÓN ENTRE VALOR_VOLTAJE/100, QUE DA COMO RESULTADO LA PARTE ENTERA Y DESCARTA LO DECIMALES
			uint8_t entero  = valor_voltaje / 100;
			//DECIMAL ALMACENA EL RESULTADO DEL OPERADOR MÓDULO, EL CUAL DEVUELVE EL RESIDUO DE LA DIVISIÓN, EXTRAE LOS DOS ÚLTIMOS DÍGITOS 
			uint8_t decimal = valor_voltaje % 100;
			// SE CREAN DOS VARIABLES DEL TIPO CHAR, PARA QUE ALMACENEN UN MÁXIMO DE 3 DÍGITOS TANTO PARA LA PARTE ENTERA COMO DECIMAL 
			char buffENT[3];
			char buffDEC[3];
			// ITOA ES UNA FUNCIÓN PARA CONVERTIR UN ENTERO A STRING, RECIBE EL ENTERO, DESTINO DE ALMACENAMIENTO Y EN QUE FORMATO SE DESEA VER, 10 PARA BASE DECIMAL 
			itoa(entero,  buffENT, 10);
			itoa(decimal, buffDEC, 10);
			// SE MUESTRA EL RESULTADO DEL VOLTAJE LEÍDO EN LA TERMINAL 
			writeString(buffENT);
			writeChar(',');
			writeString(buffDEC);
			writeString("V");
			// SE REESTABLECE LA VARIABLE ESTADO_FUNCIÓN A 0, PARA REGRESAR AL MENÚ 
			estado_funcion = 0;
			// SE DESPLIEGA EL MENÚ 
			menu();
		}
		// SI LA RESPUESTA EN MENÚ FUE 2 
		else if (bufferRX == '2')
		{
			// SE ALTERA LA VARIABLE ESTADO_FUNÇION A 2 PARA QUÉ EN LA PRÓXIMA INTERRUPCIÓN SE DESPLIEGUE EL ASCII 
			estado_funcion = 2;
			writeString("Envia un caracter:");
		}
		else
		{
			// SI EN EL MENÚ, EL USUARIO COLOCA UNA OPCIÓN INVÁLIDA, SE DESPLIEGA EL MENÚ 
			writeString("Opcion invalida");
			menu();
		}
	}
	// SI LA "MÁQUINA" ESTÁ EN ESTADO 2, ENTONCES, YA SE RECIBIÓ EL ASCII POR PARTE DEL USUARIO, Y SE LLAMA A LA FUNCIÓN DE DISPLAY ASCII
	else if (estado_funcion == 2)     
	{
		displayASCII(bufferRX);
		estado_funcion = 0;
		menu();
	}
}
ISR(ADC_vect)
{	
	// SE ALMACENA EN VALOR_ADC5 EL VALOR DEL ADC
	valor_ADC5 = ADC; 
	// EN VALOR_VOLTAJE SE ALMACENA LA CONVERSIÓN DE VOLTAJE, UL = UNSIGNED LONG, PARA QUE LA MULTIPLICACIÓN QUEPA EN 32 BITS Y TRAS LA DIVISIÓN EL RESULTADO CABE EN 16 BITS
	valor_voltaje = (valor_ADC5* 500UL)/1023; 
	
}

