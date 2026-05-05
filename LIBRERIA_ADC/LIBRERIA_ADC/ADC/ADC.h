/*
 * ADC.h
 *
 * Created: 28/04/2026 11:40:16
 *  Author: Omar Cabrera 
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
 
// VOLTAJE DE REFERENCIA 
#define APAGADO 0 
#define AVCC 1 
#define RESERVADO 2 
#define REFERENCIA_INTERNA 3 

// FORMATO DE RESULTADO 
#define J_DER 0 // JUSTIFICACIÓN A LA DERECHA 
#define J_IZQ 1 // JUSTIFICACIÓN A LA IZQUIERDA 

// CANAL ADC INICIAL 
#define ADC_0 0 
#define ADC_1 1 
#define ADC_2 2 
#define ADC_3 3 
#define ADC_4 4 
#define ADC_5 5 
#define ADC_6 6 
#define ADC_7 7
 
// INTERRUPCIONES DEL ADC 
#define INTERRUPCION 1 
#define NO_INTERRUPCION 0 

// INICIALIZACIÓN DEL ADC 
void initADC(uint8_t voltaje, uint8_t formato, uint8_t canal, uint8_t prescaler, uint8_t interrupcion);

// CAMBIA EL CANAL ACTIVO, SERÁ ÚTIL EN LA ISR PARA LEER EL ADC DE LOS DISTINTOS POTENCIÓMETROS 
void ADC_cambiar_canal(uint8_t canal);

// INICIA UNA CONVERSIÓN, ÚTIL PARA INTERRUPCIONES 
void ADC_iniciar_conversion(void); 

// INICIA CONVERSIÓN Y ESPERA EL RESULTADO (MODO SIN INTERRUPCIONES)
uint16_t ADC_leer(uint8_t canal); 

#endif /* ADC_H_ */