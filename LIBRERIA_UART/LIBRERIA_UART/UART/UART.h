/*
 * UART.h
 *
 * Created: 28/04/2026 11:31:01
 *  Author: Omar Cabrera 
 */ 

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
void initUART();
void writeChar(char c);
void writeString(char* string);



#endif /* UART_H_ */