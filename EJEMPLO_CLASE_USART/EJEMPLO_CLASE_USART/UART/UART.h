/*
 * UART.h
 *
 * Created: 23/04/2026 14:35:24
 *  Author: aleja
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
void initUART();
void writeChar(char c);
void writeString(char* string);



#endif /* UART_H_ */