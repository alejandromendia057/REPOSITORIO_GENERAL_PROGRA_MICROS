/*
 * NombreProgra.c
 *
 * Created: 6/04/2026
 * Author: Omar Cabrera 
 * Description: Contador binario de 8 bits y módulo ADC para mostrar el voltaje que está pasando en los displays
 */
/****************************************/
// Encabezado (Libraries)

#define F_CPU 1000000UL													// CONFIGURACIÓN DE RELOJ PARA QUE LOS DELAYS SEAN PRECISOS Y NO USEN POR DEFECTO 1 MHZ O 10 MHZ SEGÚN EL COMPILADOR
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/****************************************/
// Function prototypes
	
void setup();															// SETUP DE ENTRADAS Y SALIDAS 
void initADC();															// RUTINA QUE INICIALIZA ADC PARA ADC6 
void actu_leds(uint8_t valor);											// RUTINA QUE ACTUALIZARÁ LOS LEDS PB0-PB3 (0-3) y PC3-PC0 (4-7)
void initTMR2();														// RUTINA QUE INICIALIZA EL TIMER2 EN MODO NORMAL (2MS) 

// Variables Globales 

uint8_t contador = 0;													// INICIALIZA CONTADOR EN 0
uint8_t ultimo_inc = 1;													// SE INICIALIZA EL ESTADO ANTERIOR PARA EL BOTÓN PD7 EN 1 PARA QUE SIEMPRE EMPIECE COMO BOTÓN SUELTO (1) 
uint8_t ultimo_dec = 1;													// SE INICIALIZA EL ESTADO ANTERIOR PARA EL BOTÓN PC5 EN 1 PARA QUE SIEMPRE EMPIECE COMO BOTÓN SUELTO (1)
volatile uint8_t copia_low_bits_ADCH = 0;								// EN ESTA VARIABLE GUARDARÉ LOS BITS MENOS SIGNIFICATIVOS DEL ADCH PARA MANDARLO POSTERIORMENTE AL DISPLAY DEL DÍGITO 1
volatile uint8_t copia_high_bits_ADCH = 0;								// EN ESTA VARIABLE GUARDARÉ LOS BITS MÁS SIGNIFICATIVOS DEL ADCH PARA MANDARLO POSTERIORMENTE AL DISPLAY DEL DÍGITO 2
volatile uint8_t copia = 0;												// VARIABLE QUE ALMACENA EL VALOR ARROJADO POR EL ADC 
volatile uint8_t disp_selec = 0;										// ESTA VARIABLE ME SERVIRÁ PARA SABER QUE DISPLAY DEBO DE ENCENDER, 0 PARA EL DÍGITO 1 (UNIDADES) Y 1 PARA EL DÍGITO 2 (DECENAS) 

uint8_t tabla_7segmentos[] = {0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xEF, 0xF7, 0xFC, 0xB9, 0xDE, 0xF9, 0xF1};			// TABLA PARA LOS NÚMEROS EN HEX, DISPLAYS CÁTODO COMÚN 

/****************************************/
// Main Function

int main(void)
{	
	cli();
	setup();
	initADC(); 
	initTMR2(); 
	
	// HABILITACIÓN DE INTERRUPCIONES PARA EL ADC
	ADCSRA |= (1 << ADSC)|(1 << ADIE);
	
	// HABILITACIÓN DE INTERRUPCIONES PARA EL TIMER2 
	TIMSK2 |= (1 << TOIE2); 
	

	sei(); 
	while (1)
	{
		uint8_t actual_inc = (PIND & (1 << PORTD7)) ? 1:0;				// LECTURA ACTUAL DE BOTONES, DE NO ESTAR PRESIONADO EL BOTÓN SE LE ASIGNA A ACTUAL_INC=1 
		uint8_t actual_dec = (PINC & (1 << PORTC5)) ? 1:0;				// LECTURA ACTUAL DE BOTONES, DE ESTAR PRESIONADO EL BOTÓN SE LE ASGINA A ACTUAL_DEC=0 
		
		if (actual_inc == 0 && ultimo_inc == 1){						// CONDICIÓN PARA SABER SI SE DEBE INCREMENTAR O DECREMENTAR EL CONTADOR, COMPARA LA PULSACIÓN ACTUAL CON LA DEL ESTADO ANTERIOR 
			contador++;
			actu_leds(contador);
			_delay_ms(50); 
		}
		if (actual_dec == 0 && ultimo_dec == 1){
			contador--; 
			actu_leds(contador); 
			_delay_ms(50); 
		}
		ultimo_inc = actual_inc;										// ACTUALIZAMOS EL ESTADO ANTERIOR DE PULSACIÓN 
		ultimo_dec = actual_dec;
	}
}

/****************************************/
// NON-Interrupt subroutines

void setup()
{
	// PRESCALER_CPU = 16 -> F_CPU = 1 MHZ
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	
	// CONFIGURACIÓN DEL PUERTO B. SALIDAS DE LOS LEDS PB0-PB3 Y TRANSISTORES, PB4 CONTROLARÁ EL DISPLAY DEL DÍGITO 1 Y EL PB5 EL DÍGITO 2 
	DDRB |= (1 << PORTB5)|(1 << PORTB4)|(1 << PORTB3)|(1 << PORTB2)|(1 << PORTB1)|(1 << PORTB0); 
	PORTB &= ~((1 << PORTB5)|(1 << PORTB4)|(1 << PORTB3)|(1 << PORTB2)|(1 << PORTB1)|(1 << PORTB0)); 
	
	// CONFIGURACIÓN DEL PUERTO C SALIDAS DE LOS LEDS PC0-PC3, BOTÓN PULL-UP COMO ENTRADA EN PC5, LED ALARMA EN PC4 COMO SALIDA
	DDRC |= ((1 << PORTC4)|(1 << PORTC3)|(1 << PORTC2)|(1 << PORTC1)|(1 << PORTC0)); 
	PORTC &= ~((1 << PORTC4)|(1 << PORTC3)|(1 << PORTC2)|(1 << PORTC1)|(1 << PORTC0)); 
	DDRC &= ~(1 << PORTC5); 
	PORTC |= (1 << PORTC5);
	 
	// CONFIGURACIÓN DEL PUERTO D. SEGMENTOS DEL DISPLAY COMO SALIDA PD0-PD6, BOTÓN PULL-UP COMO ENTRADA EN PD7
	 
	DDRD |= (1 << PORTD6)|(1 << PORTD5)|(1 << PORTD4)|(1 << PORTD3)|(1 << PORTD2)|(1 << PORTD1)|(1 << PORTD0); 
	PORTD &= ~((1 << PORTD6)|(1 << PORTD5)|(1 << PORTD4)|(1 << PORTD3)|(1 << PORTD2)|(1 << PORTD1)|(1 << PORTD0)); 
	DDRD &= ~(1 << PORTD7);
	PORTD |= (1 << PORTD7);
	
	UCSR0B = 0x00; 
	
	
}
void initADC()
{
	// INICIALIZACIÓN DE ADMUX 
	ADMUX = 0; 
	
	// CONFIGURACIÓN DEL REGISTRO ADMUX: REFS0 Y REFS1 PARA ESTABLECER EL VOLTAJE DE REFERENCIA CON EL QUE SE TRABAJARÁ, ADLAR: JUSTIFICACIÓN A LA IZQUIERDA, MUX2: 1 PORQUE POT ESTÁ EN ADC4
	ADMUX |= (1 << REFS0)|(1 << ADLAR)|(1 << MUX2)|(1 << MUX1);
	
	// INICIALIZACIÓN DE ADCSRA 
	ADCSRA = 0; 
	
	// CONFIGURACIÓN DEL REGISTRO ADCSRA: ADEN PARA ACTIVAR EL ADC, ADPS1 Y ADPS0 PARA EL PRESCALER CON UN FACTOR DE 8 -> 1 MHZ / 8 = 125 KHZ
	ADCSRA |= (1 << ADEN)| (1 << ADPS1)| (1 << ADPS0);
	

}

void initTMR2() {
	
	// CONFIGURACIÓN DEL TIMER2 CON DELAY DE 2MS PARA MULTIPLEXAR EN MODO NORMAL 
	TCCR2A = 0;
	
	// PRESCALER DE 8 
	TCCR2B = 0; 
	TCCR2B |= (1 << CS21); 
	
	// VALOR INICIAL DE 6 PARA MODO NORMAL 
	TCNT2 = 0x6; 
	
}

void actu_leds(uint8_t valor)
{
	if (valor & (1 << 0))						// LÓGICA PARA ENCENDER O APAGAR LOS LEDS DEL BYE MENOS SIGNIFICATIVO EN PORTB 				
		PORTB |= (1 << PORTB0);
	else 
		PORTB &= ~(1 << PORTB0); 
		
	if (valor & (1 << 1))
		PORTB |= (1 << PORTB1);
	else
		PORTB &= ~(1 << PORTB1);
		
	if (valor & (1 << 2))
		PORTB |= (1 << PORTB2);
	else
		PORTB &= ~(1 << PORTB2);
	
	if (valor & (1 << 3))
		PORTB |= (1 << PORTB3);
	else
		PORTB &= ~(1 << PORTB3);
	
	if (valor & (1 << 4))						// LÓGICA PARA ENCENDER O APAGAR LEDS DEL BYTE MÁS SIGNIFICATIVO EN PORTC 
		PORTC |= (1 << PORTC3);
	else
		PORTC &= ~(1 << PORTC3);
	
	if (valor & (1 << 5))					
		PORTC |= (1 << PORTC2);
	else
		PORTC &= ~(1 << PORTC2);
	
	if (valor & (1 << 6))
		PORTC |= (1 << PORTC1);
	else
		PORTC &= ~(1 << PORTC1);
	
	if (valor & (1 << 7))
		PORTC |= (1 << PORTC0);
	else
		PORTC &= ~(1 << PORTC0);
	
}

/****************************************/
// Interrupt routines

ISR(ADC_vect){
	
	copia = ADCH; 
	copia_low_bits_ADCH = copia & 0x0F; 							// SE APLICA UNA MÁSCARA PARA SOLO QUEDARNOS CON LOS BITS MENOS SIGNIFICATIVOS 		
	copia_high_bits_ADCH = (copia >> 4);							// CORREMOS LOS 4 BITS MÁS SIGNIFICATIVOS 
	ADCSRA |= (1 << ADSC);											// SE VUELVE A ACTIVAR LA LECTURA DEL ADC 
}

ISR(TIMER2_OVF_vect){
	
	TCNT2 = 0x06;													// SE RECARGA EL VALOR INICIAL PARA HACER EL CONTEO 
	PORTB &= ~((1 << PORTB5)|(1 << PORTB4));						// SE APAGAN TODOS LOS TRANSISTORES 
	if (disp_selec == 0){
		PORTD = tabla_7segmentos[copia_low_bits_ADCH];				// MOSTRAR BYTE BAJO DEL ADC EN EL DISPLAY 1 (UNIDADES)
		PORTB |= (1 << PORTB4);										// ACTIVA EL TRANSISTOR PARA ENCENDER EL DÍGITO 1 
		disp_selec = 1;												// SE LE ASIGNA AL ÍNFICE DISP_SELEC = 1 PARA QUE LA PRÓXIMA INTERRUPCIÓN SE ENCIENDA EL DISPLAY 2 
	}  else {
		PORTD = tabla_7segmentos[copia_high_bits_ADCH];				// MOSTRAR BYTE ALTO DEL ADC EN EL DISPLAY 2 (DECENAS)
		PORTB |= (1 << PORTB5);										// ACTIVA EL TRANSISTOR PARA ENCENDER EL DÍGITO 2 
		disp_selec = 0;												// SE LE ASIGNA AL ÍNDICE DISP_SELEC = 0 PARA QUE LA PRÓXIMA INTERRUPCIÓN SE ENCIENDE EL DISPLAY 1
	}
	
	// RUTINA PARA REVISAR CADA 2 MS SI EL VALOR DEL ADC ES MAYOR QUE EL DEL CONTADOR 
	if ( copia > contador){
		PORTC |= (1 << PORTC4); 
	} else {
		PORTC &= ~(1 << PORTC4);
	} 
	if (copia == contador){
		PORTC &= ~(1 << PORTC4); 
	}
	
}
