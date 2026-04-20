/*
 * NombreProgra.c
 *
 * Created: 
 * Author: 
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "PWM1/PWM1.h"

/****************************************/
// Function prototypes

void setup();
void initADC(); 
uint16_t lectura_adc(uint8_t canal);									// NO SE UTILIZA SI SE HACE POR INTERRUPCIÓN

// Variables globales

volatile uint8_t copia_low_bits_ADC = 0;								// EN ESTA VARIABLE GUARDARÉ LOS BITS MENOS SIGNIFICATIVOS DEL ADC
volatile uint8_t copia_high_bits_ADC = 0;								// EN ESTA VARIABLE GUARDARÉ LOS BITS MÁS SIGNIFICATIVOS DEL ADC
volatile uint16_t valor_ADC = 0;											// VALOR COMPLETO DEL ADC DE 10 BITS (0-1023)
volatile uint16_t pulso_servo = 1500;									// ANCHO DE PULSO EN MICRO SEGUNDO PARA QUE EL SERVO INICIO CENTRADO

/****************************************/
// Main Function

int main(void)
{
	cli();
	setup();
	initADC();
	
	// INICIALIZA EL PWM1A PARA SERVO: 50 HZ, TOP = 19,999, PRESCALER = 1, F_CLK = 1 MHZ
	initPWM1A(NO_INVERTIDO, MODO_FAST_PWM_ICR, 1, 19999);
	
	// HABILITACIÓN DE INTERRUPCIONES PARA EL ADC
	ADCSRA |= (1 << ADSC)|(1 << ADIE);
	
	sei();
	
	
	while (1)
	{
		
	}
}


/****************************************/
// NON-Interrupt subroutines

void setup()
{
	CLKPR = (1 << CLKPCE);	// HABILITA CAMBIO DE PRESCALER 
	CLKPR = (1 << CLKPS2);	// RELOJ A 1MHZ 
}

void initADC()
{
	// INICIALIZACIÓN DE ADMUX
	ADMUX = 0;
	
	// CONFIGURACIÓN DEL REGISTRO ADMUX: REFS0 PARA ESTABLECER EL VOLTAJE DE REFERENCIA, ADLAR: JUSTIFICACIÓN A LA DERECHA, MUX2: 1 PORQUE POT ESTÁ EN ADC4
	ADMUX |= (1 << REFS0)|(1 << MUX2);
	
	// INICIALIZACIÓN DE ADCSRA
	ADCSRA = 0;
	
	// CONFIGURACIÓN DEL REGISTRO ADCSRA: ADEN PARA ACTIVAR EL ADC, ADPS1 Y ADPS0 PARA EL PRESCALER CON UN FACTOR DE 8 -> 1 MHZ / 8 = 125 KHZ
	ADCSRA |= (1 << ADEN)| (1 << ADPS1)| (1 << ADPS0);
	

}

/****************************************/
// Interrupt routines

ISR(ADC_vect)
{
	// PRIMERO LECTURA DEL ADCL Y LUEGO DEL ADCH 
	copia_low_bits_ADC = ADCL; 
	copia_high_bits_ADC = ADCH; 
	
	// COMBINACIÓN PARA OBTENER EL VALOR DE 10 BITS (0-1023)
	valor_ADC = (copia_high_bits_ADC << 8)|copia_low_bits_ADC; 
	
	// SE MAPEA EL VALOR DEL ADC AL RANGO DEL SERVO (1000 MICRO SEGUNDOS A 2000 MICROSEGUNDOS)
	pulso_servo = 1000 + ((uint32_t)valor_ADC*1000)/1023; 
	
	// SE ACTUALIZA EL REGISTRO DE COMPARACIÓN DEL PWM 
	updateDutyCycle1A(pulso_servo);
	
	// INICIAR NUEVA CONVERSIÓN DEL ADC PARA PRÓXIMA LECTURA 
	ADCSRA |= (1 << ADSC); 
}

