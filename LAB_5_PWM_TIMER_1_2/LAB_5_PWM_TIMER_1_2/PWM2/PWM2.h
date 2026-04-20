/*
 * PWM2.h
 *
 * Created: 14/04/2026 14:28:06
 *  Author: aleja
 */ 

#ifndef PWM2_H_
#define PWM2_H_

#include <avr/io.h>

#define INVERTIDO     1
#define NO_INVERTIDO  0

#define FAST_PWM      0
#define PHASE_PWM_8BIT     1
#define PHASE_PWM_TOP_OCR2A 2

void initPWM2A(uint8_t invertido, uint8_t modo, uint16_t prescaler);
void initPWM2B(uint8_t invertido, uint8_t modo, uint16_t prescaler);

// FUNCIÓN ESPECÍFICA PARA CONTROLAR EL SERVO MOTOR CON PWM2 HACIENDO USO DE OCR2A COMO TOP Y OCR2B PARA VARIAR EL CICLO DE TRABAJO 
void initPWM2_Servo(uint16_t prescaler, uint8_t top);

void updateDutyCycle2A(uint8_t ciclo);
void updateDutyCycle2B(uint8_t ciclo);


#endif