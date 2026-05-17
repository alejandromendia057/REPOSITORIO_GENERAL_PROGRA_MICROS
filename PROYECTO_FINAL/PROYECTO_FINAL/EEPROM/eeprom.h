/*
 * eeprom.h
 *
 * Created: 23/04/2026 15:09:10
 *  Author: Omar Cabrera 
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

void writeEEPROM(uint16_t direccion, uint8_t dato);
uint8_t readEEPROM(uint16_t direccion);


#endif /* EEPROM_H_ */