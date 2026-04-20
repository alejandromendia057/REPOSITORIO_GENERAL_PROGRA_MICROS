/*
 * NombreProgra.c
 *
 * Created: 
 * Author: Omar Cabrera
 * Description: Laboratorio 5, control de un servo motor utilizando PWM del timer 2
 */
/****************************************/
// Encabezado (Libraries)

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "PWM1/PWM1.h"
#include "PWM2/PWM2.h"
#include "TIMER0_INTERRUPCION/TIMER0_INTERRUPCION.h"

// PROTOTIPOS 
void setup(void);
void initADC(void);

// VARIABLES GLOBALES 
volatile uint8_t  canal_actual = 0;      // VARIABLE PARA EMPEZAR A HACER LECTURA EN EL CANAL ADC4 
volatile uint8_t copia_adc4_ADCH = 0;	 // ALMACENA BYTE ALTO DEL ADC PARA EL POT4 
volatile uint8_t copia_adc4_ADCL = 0;	// ALMACENA BYTE BAJO DEL ADC PARA EL POT4 
volatile uint8_t copia_adc5_ADCH = 0;	 // ALMACENA BYTE ALTO DEL ADC PARA EL POT5
volatile uint8_t copia_adc5_ADCL = 0;	// ALMACENA BYTE BAJO DEL ADC PARA EL POT5
volatile uint8_t copia_adc6_ADCH = 0;	 // ALMACENA BYTE ALTO DEL ADC PARA EL POT6
volatile uint8_t copia_adc6_ADCL = 0;	// ALMACENA BYTE BAJO DEL ADC PARA EL POT6
volatile uint16_t valor_ADC4 = 0;		// ALMACENA LOS 10 BITS DEL ADC4
volatile uint16_t valor_ADC5 = 0;		// ALMACENA LOS 10 BITS DEL ADC5
volatile uint16_t pulso_servo4 = 3000;	// VARIABLE QUE CONTROLA EL CICLO DE TRABAJO PARA EL POT4, EMPIEZA CENTRADO 
volatile uint16_t pulso_servo5 = 12; // VARIABLE QUE CONTROLA EL CICLO DE TRABAJO PARA EL POT5, EMPIEZA CENTRADO

// VARIABLES PARA CONTROLAR EL PWM DE FORMA MANUAL, LED EN PD2 
volatile uint8_t contador_pwm = 0; 
volatile uint8_t umbral_led = 0; // RANGO ESCOGIDO [0-100]
volatile uint16_t valor_ADC6 = 0; 

int main(void)
{
	cli();
	setup();
	initADC();
	DDRD |= (1 << DDD2);
	
	// INICIALIZACIÓN DEL TIMER0 EN MODO CTC, PRESCALER DE 8, OCR0A = 200, INTERRUPCIÓN CADA 100 MICRO SEGUNDOS
	timer0_init(TIMER0_MODO_CTC, 8, 200);
	
	// HABILITAR INTERRUPCIÓN POR COMPARACIÓN A DEL TIMER0 
	timer0_habilitar_interrupcion_compA(); 
	
	// TIMER 1, PARA SERVO EN PB1 (OC1A), NO INVERTIDO, MODO FAST PWM CON ICR, PRESCALER DE 8, ICR1 = 39,999
	initPWM1A(NO_INVERTIDO, MODO_FAST_PWM_ICR, 8, 39999);

	// TIMER 2, PARA SERVO EN PD3 (OC2B), NO INVERTIDO, MODO FAST PWM CON TOP EN OCRA, NO INVERTIDO, PRESCALER DE 1024, OCRA = 156
	initPWM2_Servo(1024, 156);

	// HABILITAR INTERRUPCIONES PARA EL ADC 
	ADCSRA |= (1 << ADIE) | (1 << ADSC);

	sei();

	while (1) { }
}

void setup(void)
{
	// SIN PRESCALER DEL RELOJ PRINCIPAL, SE QUEDA EN 16 MHZ 
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;   
}

void initADC(void)
{
	// INICIALIZACIÓN DE ADMUX 
	ADMUX = 0; 
	
	// VOLTAJE DE REFERENCIA, JUSTIFICACIÓN A LA DERECHA, ADC4 ACTIVADO 
	ADMUX |= (1 << REFS0)|(1 << MUX2);        
	
	// INICIALIZACIÓN DE ADCSRA 
	ADCSRA = 0; 
	
	// CONFIGRUACIÓN DEL REGISTRO ADCSRA: ADEN PARA ACTIVAR EL ADC, ADPS2,1,0 PARA TENER 125 KHZ 
	 ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); 
}

// ISR DEL ADC 
ISR(ADC_vect)
{
	switch(canal_actual) {
		
		case 0:
		// LECTURA DEL ADCL Y ADCH PARA EL POT4
		copia_adc4_ADCL = ADCL;
		copia_adc4_ADCH = ADCH;
		
		// COMBINACIÓN PARA OBTENER LOS 10 BITS
		valor_ADC4 = (copia_adc4_ADCH << 8)|copia_adc4_ADCL;
		
		// SE MAPEA EL VALOR DEL ADC AL RANGO DEL SERVO CONTROLADO POR EL TIMER 1
		pulso_servo4 = 2000 + ((uint32_t)valor_ADC4*2000)/1023;
		
		// SE LLAMA A LA FUNCIÓN DE LA LIBRERÍA DEL PWM1 PARA ACTUALIZAR EL CICLO DE TRABAJO
		updateDutyCycle1A(pulso_servo4);
		
		// CAMBIAR A CANAL 5
		ADMUX = (1 << REFS0) | (1 << MUX2)|(1 << MUX0);
		canal_actual = 1;
		
		// INICIAR NUEVA CONVERSIÓN DEL ADC PARA PRÓXIMA LECTURA
		ADCSRA |= (1 << ADSC);
		break; 
		
		case 1: 
		// LECTURA DEL ADCL Y ADCH PARA EL POT5
		copia_adc5_ADCL = ADCL;
		copia_adc5_ADCH = ADCH;
		
		// COMBINACIÓN PARA OBTENER LOS 10 BITS
		valor_ADC5 = (copia_adc5_ADCH << 8)|copia_adc5_ADCL;
		
		// SE MAPEA EL VALOR DEL ADC AL RANGO DEL SERVO CONTROLADO POR EL TIMER 2
		pulso_servo5 = 8 + ((uint32_t)valor_ADC5*8)/1023;
		
		// SE LLAMA A LA FUNCIÓN DE LA LIBRERÍA DEL PWM2 PARA ACTUALIZAR EL CICLO DE TRABAJO
		updateDutyCycle2B(pulso_servo5);
		
		// CAMBIAR A CANAL 6
		ADMUX = (1 << REFS0) | (1 << MUX2)|(1 << MUX1);
		canal_actual = 2;
		
		// INICIAR NUEVA CONVERSIÓN DEL ADC PARA PRÓXIMA LECTURA
		ADCSRA |= (1 << ADSC);
		break; 
		
		case 2: 
		// LECTURA DEL ADCL Y ADCH PARA EL POT4
		copia_adc6_ADCL = ADCL;
		copia_adc6_ADCH = ADCH;
		
		// COMBINACIÓN PARA OBTENER LOS 10 BITS
		valor_ADC6 = (copia_adc6_ADCH << 8)|copia_adc6_ADCL;
		
		// SE MAPEA EL VALOR DEL ADC AL UMBRAL DEL LED 
		umbral_led = (uint8_t)(((uint32_t)valor_ADC6*100)/1023); 
		
		// CAMBIAR A CANAL 4
		ADMUX = (1 << REFS0) | (1 << MUX2);
		canal_actual = 0;
		
		// INICIAR NUEVA CONVERSIÓN DEL ADC PARA PRÓXIMA LECTURA
		ADCSRA |= (1 << ADSC);
	}
	
}

// ISR DEL TIMER0 
ISR(TIMER0_COMPA_vect){
	if (contador_pwm == 0){
		PORTD |= (1 << PORTD2); // EL LED SE ENCIENDE EN ALTO 
	}
	if (contador_pwm == umbral_led){
		PORTD &= ~(1 << PORTD2); // APAGAR EL LED EN BAJO 
	}
	contador_pwm ++;
	if (contador_pwm >=100){
		contador_pwm = 0; 
	}
}