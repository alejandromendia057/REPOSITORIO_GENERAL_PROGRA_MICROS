/*
 * PWM0.h
 *
 * Created: 28/04/2026 10:30:04
 *  Author: Omar Cabrera 
 */ 


#ifndef PWM0_H_
#define PWM0_H_

#include <avr/io.h>

#define INVERTIDO     1
#define NO_INVERTIDO  0

#define FAST_PWM      0
#define PHASE_PWM_8BIT     1
#define PHASE_PWM_TOP_OCR0A 2

void initPWM0A(uint8_t invertido, uint8_t modo, uint16_t prescaler);
void initPWM0B(uint8_t invertido, uint8_t modo, uint16_t prescaler);

// FUNCIÓN ESPECÍFICA PARA CONTROLAR EL SERVO MOTOR CON PWM0 HACIENDO USO DE OCR0A COMO TOP Y OCR0B PARA VARIAR EL CICLO DE TRABAJO
void initPWM0_Servo(uint16_t prescaler, uint8_t top);
void updateServo0B(uint8_t ciclo); 

void updateDutyCycle0A(uint8_t ciclo);
void updateDutyCycle0B(uint8_t ciclo);


#endif