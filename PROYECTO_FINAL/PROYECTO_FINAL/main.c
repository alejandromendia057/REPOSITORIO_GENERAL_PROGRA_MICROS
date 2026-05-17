/*
 * PROYECTO_FINAL.c
 *
 * Created: 5/05/2026 16:08:15
 * Author : Omar Cabrera 
 */ 

/* MAPA DE CONEXIONES:

	SERVOS: SERVO1 EN D9 (OC1A), SERVO2: D10(OC1B), SERVO3: D5(0C0B), SERVO4: D3 (OC2B)
	POTENCIÓMETROS: A0, A1, A2, A3 
	LEDS: MANUAL (PD2), EEPROM (PD4), UART(PD6)
	BOTONES: MODO (PB0), GUARDAR(PB4), REPRODUCIR(PD7)
	UART: RX(PD0), TX(PD1)
*/
/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ADC/ADC.h"
#include "EEPROM/eeprom.h"
#include "PWM2/PWM2.h"
#include "UART/UART.h"
#include "PWM1/PWM1.h"
#include "PWM0/PWM0.h"

// MODOS DE LA GARRA 
#define M_MANUAL 0
#define M_EEPROM 1 
#define M_UART 2

// LEDS DE MODO (SALIDAS)
#define LED_MANUAL DDD2 
#define LED_EEPROM DDD4
#define LED_UART DDD6 

#define LED_MANUAL_BIT PD2 
#define LED_EEPROM_BIT PD4
#define LED_UART_BIT PD6 

// BOTONES (ENTRADAS CON PULL-UP)
#define B_MODO DDB0 
#define B_GUARDAR DDB4 
#define B_REPRODUCIR DDD7 

#define B_MODO_BIT PB0 
#define B_GUARDAR_BIT PB4 
#define B_REPRODUCIR_BIT PD7

// CONSTANTES DE SERVO PARA EL TIMER1 (FAST PWM, ICR1 = 39,999, PRESCALER =8)
#define SERVO16_MIN 2000U 
#define SERVO16_CTR 3000U 
#define SERVO16_MAX 4000U 

// CONSTANTES DE SERVO PARA EL TIMER 0 Y 2 (PHASE CORRECT, OCRNA = 156 (TOP), PRESCALER = 1024)
#define SERVO8_TOP 156U 
#define SERVO8_MIN 8U 
#define SERVO8_CTR 12U 
#define SERVO8_MAX 16U 

// ESTRUCTURA DEL EEPROM  
#define MAX_POSICIONES 10U 
#define BYTES_POR_POS 6U 
#define DIR_CONTADOR 60U 

// VARIABLES GLOBALES 

// MODO ACTUAL 
volatile uint8_t M_ACTUAL = M_MANUAL; 

// PULSOS ACTUALES DE CADA SERVO 
volatile uint16_t PULSO_S1 = SERVO16_CTR;				// TIMER 1 CANAL A 
volatile uint16_t PULSO_S2 = SERVO16_CTR;				// TIMER 1 CANAL B 
volatile uint8_t PULSO_S3 = SERVO8_CTR;					// TIMER0 CANAL B 
volatile uint8_t PULSO_S4 = SERVO8_CTR;					// TIMER2 CANAL B 

// BANDERAS DE BOTONES 
volatile uint8_t FL_MODO = 0; 
volatile uint8_t FL_GUARDAR = 0; 
volatile uint8_t FL_REPRODUCIR = 0; 

// ESTADOS ANTERIORES DE BOTONES, DETECCIÓN DE  FLANCOS 
uint8_t EST_B_MODO = 1; 
uint8_t EST_B_GUARDAR = 1; 
uint8_t EST_B_REPRODUCIR = 1; 

// ÍNDICES DE EEPROM 
uint8_t IDX_GUARDAR = 0;								// PRÓXIMA RANURA A GUARDAR 
uint8_t IDX_REPRODUCIR = 0;								// PRÓXIMA POSICIÓN A REPRODUCIR 

// UART - BUFFER DE COMANDOS 
char BFF_RX[32]; 
uint8_t IDX_BFF = 0; 
uint8_t CMD_LISTO = 0; 


/****************************************/
// PROTOTIPO DE FUNCIONES

void setup(void);
void setup_pines(void); 
void setup_pwm(void); 
void setup_adc(void);
void setup_uart(void);
void setup_eeprom(void);

// FUNCIONES DE MODOS 

void modo_manual(void);
void modo_eeprom(void); 
void modo_uart(void); 

// Funciones para servos 

uint16_t mapeo16(uint16_t val);
uint8_t mapeo8(uint16_t val);
void actualizar_servos(void);

// Funciones para botones y leds 
 
 void verificar_botones(void);
 void actualizar_leds(void);
 void cambiar_Modo(void);
 
 // EEPROM 
 
 void guardar_posicion(void);
 void reproducir_siguiente(void); 
 void cargar_posicion(uint8_t num); 
 
 // UART - ADAFRUIT 
 
 void procesar_comando(void);
 void enviar_posicion(void);

/****************************************/
// Main Function

int main(void)
{
	cli();
	setup();
	sei();
	
	while (1){
	
	// VERIFICACIÓN DE BOTONES EN CADA CICLO 
	verificar_botones(); 
	
	// CAMBIO DE MODO SI EL BOTÓN FUE PRESIONADO 
	if(FL_MODO)
	{
		cambiar_Modo();
		FL_MODO = 0; 
	}
	
	// EJECUCIÓN DE LA LÓGICA DEL MODO CORRESPONDIENTE 
	switch(M_ACTUAL)
	{
		case M_MANUAL: modo_manual(); 
		break; 
		case M_EEPROM: modo_eeprom(); 
		break;
		case M_UART: modo_uart(); 
		break;
		default: modo_manual();
		break;
	}
	
	// PROCESAMIENTO DEL COMANDO UART 
	if(CMD_LISTO)
	{
		procesar_comando(); 
		CMD_LISTO = 0; 
		IDX_BFF = 0; 
	}
	}
		
}

/****************************************/
// NON-Interrupt subroutines

void setup(void)
{
	setup_pines();
	setup_pwm();
	setup_adc();
	setup_uart();
	setup_eeprom();
	
	// SE MUESTRA MODO INICIAL 
	actualizar_leds(); 
	writeString("INICIALIZACIÓN DE LA GARRA, MODO: MANUAL\r\n"); 
}

void setup_pines(void)
{
	// LEDS COMO SALIDAS Y APAGADOS AL INICIO 
	DDRD |= (1 << LED_MANUAL)|(1 << LED_EEPROM)|(1 << LED_UART); 
	PORTD &= ~((1 << LED_MANUAL_BIT)|(1 << LED_EEPROM_BIT)|(1 << LED_UART_BIT));
	
	// BOTONES COMO ENTRADAS CON PULLUP 
	DDRB &= ~((1 << B_MODO)|(1 << B_GUARDAR));
	PORTB |= (1 << B_MODO_BIT)|(1 << B_GUARDAR_BIT);
	
	DDRD &= ~(1 << B_REPRODUCIR);
	PORTD |= (1 << B_REPRODUCIR_BIT);
}

void setup_pwm(void)
{
	initPWM1AB(NO_INVERTIDO, NO_INVERTIDO, MODO_FAST_PWM_ICR, 8, 39999);
	updateDutyCycle1A(SERVO16_CTR);
	updateDutyCycle1B(SERVO16_CTR);
	
	initPWM0_Servo(1024, SERVO8_TOP);
	updateDutyCycle0B(SERVO8_CTR); 
	
	initPWM2_Servo(1024, SERVO8_TOP); 
	updateDutyCycle2B(SERVO8_CTR); 
}

void setup_adc(void)
{
	initADC(AVCC, J_DER, ADC_0, 128, NO_INTERRUPCION);
}

void setup_uart(void)
{
	initUART(); 
}	

void setup_eeprom(void)
{
	// SE LEE EL CONTADOR DE POSICIONES GUARDADAS 
	uint8_t CNT = readEEPROM(DIR_CONTADOR); 
	if (CNT == 0xFF) CNT = 0;							// PRIMERA VEZ QUE SE USA LA EEPROM 
	IDX_GUARDAR = CNT; 
}

uint16_t mapeo16(uint16_t val)
{
	// SE HACE EL MAPEO DEL VALOR LEÍDO EN EL ADC (0-1023) AL TIMER1 (2000-4000)
	return SERVO16_MIN + (uint16_t)(((uint32_t)val * (SERVO16_MAX - SERVO16_MIN))/1023);
}

uint8_t mapeo8(uint16_t val)
{
	// SE HACE EL MAPEO DEL VALOR LEÍDO EN EL ADC (0-1023) AL TIMER0/TIMER2 (8-16)
	return SERVO8_MIN + (uint8_t)(((uint32_t)val * (SERVO8_MAX - SERVO8_MIN))/1023); 
}

void actualizar_servos(void)
{
	updateDutyCycle1A(PULSO_S1); 
	updateDutyCycle1B(PULSO_S2);
	updateDutyCycle0B(PULSO_S3);
	updateServo2B(PULSO_S4); 
}

void modo_manual(void)
{
	// SE LEEN LOS 4 POTENCIÓMETROS Y SE MAPEA AL RANGO DE CADA SERVOMOTOR
	// ADC_leer() ES POLLING: CAMBIA DE CANAL, INICIA, ESPERA Y RETORNA 
	
	PULSO_S1 = mapeo16(ADC_leer(ADC_0));							// PONTECIÓMETRO 1 CONTROLA AL SERVO 1 (TIMER1A)
	PULSO_S2 = mapeo16(ADC_leer(ADC_1));							// PONTECIÓMETRO 2 CONTROLA AL SERVO 2 (TIMER1B)
	PULSO_S3 = mapeo8(ADC_leer(ADC_2));								// PONTECIÓMETRO 3 CONTROLA AL SERVO 3 (TIMER0B)
	PULSO_S4 = mapeo8(ADC_leer(ADC_3));								// PONTECIÓMETRO 4 CONTROLA AL SERVO 4 (TIMER2B)
	
	actualizar_servos(); 
	
	// SE GUARDAN LAS POSICIONES SI EL BOTÓN ACTIVÓ LA FLAG 
	if (FL_GUARDAR)
	{
		guardar_posicion(); 
		FL_GUARDAR = 0; 
	}
	_delay_ms(20); 
}

// MODO EEPROM / REPRODUCCIÓN DE LAS POSICIONES 
void modo_eeprom(void)
{
	if (FL_REPRODUCIR)
	{
		reproducir_siguiente(); 
		FL_REPRODUCIR = 0; 
	}
}	

// MODO UART / ADAFRUIT 
void modo_uart(void)
{
	// LA RECEPCIÓN DE LA INFORMACIÓN OCURRE EN LA ISR DE USART_RX_vect
}

// FUNCIONES DE EEPROM 

void guardar_posicion(void)
{
	if(IDX_GUARDAR >= MAX_POSICIONES)
	{
		writeString("EEPROM LLENA\r\n");
		return; 
	}
	uint16_t DIR = (uint16_t)IDX_GUARDAR * BYTES_POR_POS; 
	
	// SERVO1 Y SERVO2 AL SER DE 16 BITS SE GUARDAN HIGH Y LOW POR SEPARADO 
	writeEEPROM(DIR, (uint8_t)(PULSO_S1 & 0xFF));
	writeEEPROM(DIR +1, (uint8_t)(PULSO_S1 >> 8));
	writeEEPROM(DIR +2, (uint8_t)(PULSO_S2 & 0xFF)); 
	writeEEPROM(DIR +3, (uint8_t)(PULSO_S2 >> 8)); 
	
	// SERVO3 Y SERVO4 SON DIRECTOS 
	writeEEPROM(DIR +4, PULSO_S3); 
	writeEEPROM(DIR +5, PULSO_S4); 
	
	// SE ACTUALIZA EL CONTADOR 
	IDX_GUARDAR++; 
	writeEEPROM(DIR_CONTADOR, IDX_GUARDAR); 
	writeString("POSICION GUARDADA\r\n");
}

void cargar_posicion(uint8_t NUM)
{
	uint16_t DIR = (uint16_t)NUM * BYTES_POR_POS; 
	
	PULSO_S1 = readEEPROM(DIR)|((uint16_t)readEEPROM(DIR + 1) << 8);
	PULSO_S2 = readEEPROM(DIR +2)|((uint16_t)readEEPROM(DIR +3) << 8);
	PULSO_S3 = readEEPROM(DIR +4);
	PULSO_S4 = readEEPROM(DIR +5); 
	
	actualizar_servos(); 
}

void reproducir_siguiente(void)
{
	uint8_t TOTAL = readEEPROM(DIR_CONTADOR);
	if (TOTAL == 0 || TOTAL == 0xFF)
	{
		writeString("SIN POSICIONES\r\n"); 
		return; 
	}
	if(IDX_REPRODUCIR >= TOTAL) IDX_REPRODUCIR = 0; 
	
	cargar_posicion(IDX_REPRODUCIR); 
	IDX_REPRODUCIR++; 
	writeString("REPRODUCIENDO\r\n"); 
	
}

// FUNCIONES DE BOTONES Y LEDS 

void verificar_botones(void)
{
	uint8_t est_modo = (PINB & (1 << B_MODO_BIT)) ? 1:0; 
	uint8_t est_guardar = (PINB & (1 << B_GUARDAR_BIT)) ? 1:0;
	uint8_t est_reproducir = (PIND & (1 << B_REPRODUCIR_BIT)) ? 1:0;
	
	// FLANCO DESCENDENTE 
	if (EST_B_MODO == 1 && est_modo == 0)
	{
		_delay_ms(30); 
		if ((PINB & (1 << B_MODO_BIT)) == 0) FL_MODO = 1; 
	}  
	if (EST_B_GUARDAR == 1 && est_guardar == 0)
	{
		_delay_ms(30);
		if((PINB & (1 << B_GUARDAR_BIT)) == 0) FL_GUARDAR = 1; 
	}
	if (EST_B_REPRODUCIR == 1 && est_reproducir == 0)
	{
		_delay_ms(30); 
		if((PIND & (1 << B_REPRODUCIR_BIT)) == 0) FL_REPRODUCIR = 1; 
	}
	EST_B_MODO = est_modo; 
	EST_B_GUARDAR = est_guardar;  
	EST_B_REPRODUCIR = est_reproducir;  
}

void actualizar_leds(void)
{
	// SE APAGAN TODOS 
	PORTD &= ~((1 << LED_MANUAL_BIT)|(1 << LED_EEPROM_BIT)|(1 << LED_UART_BIT)); 
	
	// ENCENDER EL DEL MODO ACTUAL 
	switch(M_ACTUAL)
	{
		case M_MANUAL: PORTD |= (1 << LED_MANUAL_BIT); 
		break; 
		case M_EEPROM: PORTD |= (1 << LED_EEPROM_BIT);
		break; 
		case M_UART: PORTD |= (1 << LED_UART_BIT);
		break; 
	}
}

void cambiar_Modo(void)
{
	M_ACTUAL = (M_ACTUAL + 1)%3; 
	if (M_ACTUAL == M_EEPROM) IDX_REPRODUCIR = 0;				// SE REINICIA LA REPDORUCCIÓN 
	actualizar_leds();
	switch(M_ACTUAL)
	{
		case M_MANUAL: writeString("MODO: MANUAL\r\n");
		break; 
		case M_EEPROM: writeString("MODO: EEPROM\r\n");
		break; 
		case M_UART: writeString("MODO: UART\r\n");
		break; 
	}
}

// FUNCIONES UART / ADAFRUIT 

void procesar_comando(void)
{
	writeString("CMD:");
	writeString(BFF_RX);
	writeString("\r\n");
}

void enviar_posicion(void)
{
	
}
/****************************************/
// Interrupt routines

ISR(USART_RX_vect)
{
	char c = UDR0; 
	if (c == '\r' || c == '\n')
	{
		if(IDX_BFF > 0)
		{
			BFF_RX[IDX_BFF] = '\0';
			CMD_LISTO = 1; 
		}
	}
	else if(IDX_BFF < 31)
	{
		BFF_RX[IDX_BFF++] = c; 
		writeChar(c); 
	}
}