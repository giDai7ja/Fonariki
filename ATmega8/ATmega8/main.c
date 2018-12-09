/*
* ATmega8.c
*
* Created: 24.11.2018 13:25:45
* Author : Пеппа
*/

#include <avr/io.h>
#include <avr/interrupt.h>

//#define F_CPU 8000000UL

unsigned long int TIK;

ISR(TIMER0_OVF_vect){
	TIK++;
}






int main(void)
{
	DDRB = 0xff;  // PORTB as OUTPUT
	DDRD = 0xff;  // PORTB as OUTPUT
	PORTB = 0x00;
	PORTD = 0x00;
	
	TCCR0 = (1<<CS00)|(1<<CS02);
	TIMSK = (1<<TOIE0);
	
	
	sei();
	
	/* Replace with your application code */
	while (1)
	{
		
	}
}

