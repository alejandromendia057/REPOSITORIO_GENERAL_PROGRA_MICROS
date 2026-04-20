/*
 * TIMER0_INTERRUPCION.h
 *
 * Created: 19/04/2026 21:29:55
 *  Author: aleja
 */ 


#ifndef TIMER0_INTERRUPCION_H_
#define TIMER0_INTERRUPCION_H_

#include <avr/io.h>

// MODOS DE OPERACIÓN DEL TIMER 0 

#define TIMER0_MODO_NORMAL 0 
#define TIMER0_MODO_CTC 1 

// PREESCALERS DEL TIMER 0 

#define TIMER0_PRESCALER_1 1 
#define TIMER0_PRESCALER_8 8
#define TIMER0_PRESCALER_64 64 
#define TIMER0_PRESCALER_256 256  
#define TIMER0_PRESCALER_1024 1024

// FUNCIÓN QUE INCIALIZA EL TIMER0 EN EL MODO Y PRESCALERS ESPECIFICADOS 
void timer0_init(uint8_t modo, uint16_t prescaler, uint8_t ocr0a);
 
 // FUNCIÓN PARA HABILITAR LA INTERRUPCIÓN POR COMPARACIÓN A DEL TIMER0 
 void timer0_habilitar_interrupcion_compA(void);
 
 // FUNCIÓN PARA DESHABILITAR LA INTERRUPCIÓN POR COMPARACIÓN A DEL TIMER0 
 void timer0_deshabilitar_interrupcion_compA(void);
 
 // FUNCIÓN PARA HABILITAR LA INTERRUPCIÓN POR DESBORDAMIENTO DEL TIMER0 (MODO NORMAL) 
 void timer0_habilitar_interrupcion_ovf(void);
 
 // FUNCIÓN PARA DESHABILITAR LA INTERRUPCIÓN POR DESBORDAMIENTO DEL TIMER0 (MODO NORMAL) 
 void timer0_deshabilitar_interrupcion_ovf(void);
 
 // FUNCIÓN PARA REINICIAR EL CONTADOR TCNT A 0 
 void timer0_reset_contador(void);
 
 // FUNCIÓN QUE OBTIENE EL VALOR ACTUAL DEL CONTADOR TCNT0
 uint8_t timer0_get_contador(void); 

#endif /* TIMER0_INTERRUPCION_H_ */