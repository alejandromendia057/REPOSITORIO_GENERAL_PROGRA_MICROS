/*
 * NombreProgra.c
 *
 * Created: 
 * Author: 
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h> 
#include "PWM/PWM0.h"

/****************************************/
// Function prototypes

void setup();

/****************************************/
// Main Function

int main(void)
{
	uint8_t duty = 127; 
	setup(); 
	initPWM0A(no_invertir, fastPWM, 256); 
	initPWM0B(invertir, fastPWM, 256); 
	while(1)
	{
		updateDutyCycle0A(duty);
		updateDutyCycle0B(duty);
		duty++;
		_delay_ms(1);
	}
}

/****************************************/
// NON-Interrupt subroutines

void setup()
{
	CLKPR = (1 << CLKPCE);	// HABILITA CAMBIO DE PRESCALER 
	CLKPR = (1 << CLKPS2);	// RELOJ A 1MHZ 
}


/****************************************/
// Interrupt routines