/*
* ATmega8.c
*
* Created: 24.11.2018 13:25:45
* Author : Peppa
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

//#define F_CPU 8000000UL
unsigned char D1 EEMEM;

unsigned long int SYS_TIK;
unsigned long int TurnTime;

unsigned char StepTurn = 0;
unsigned char TurnIndex = 0;

const char turn[64] = {
	0b00000000, 0b00000001,
	0b00000000, 0b00000011,
	0b00000000, 0b00000111,
	0b00000000, 0b00001111,
	0b00000000, 0b00011111,
	0b00000000, 0b00111111,
	0b00000000, 0b01111111,
	0b00000000, 0b11111111,
	0b00000001, 0b11111111,
	0b00000011, 0b11111111,
	0b00000111, 0b11111111,
	0b00001111, 0b11111111,
	0b00011111, 0b11111111,
	0b00111111, 0b11111111,
	0b01111111, 0b11111111,
	0b11111111, 0b11111111,
	0b11111111, 0b11111110,
	0b11111111, 0b11111100,
	0b11111111, 0b11111000,
	0b11111111, 0b11110000,
	0b11111111, 0b11100000,
	0b11111111, 0b11000000,
	0b11111111, 0b10000000,
	0b11111111, 0b00000000,
	0b11111110, 0b00000000,
	0b11111100, 0b00000000,
	0b11111000, 0b00000000,
	0b11110000, 0b00000000,
	0b11100000, 0b00000000,
	0b11000000, 0b00000000,
	0b10000000, 0b00000000,
	0b00000000, 0b00000000
};

ISR(TIMER0_OVF_vect){
	SYS_TIK++;
}

void Effect(void);
void AutoAdjust(void);


int main(void)
{
	unsigned char d = eeprom_read_byte(&D1);
	PORTB = 0x00;
	PORTD = 0x00;
	DDRB = 0xff;  // PORTB as OUTPUT
	DDRD = 0xff;  // PORTD as OUTPUT

	
	TCCR0 = (1<<CS00)|(1<<CS01); // CLK/64
	TIMSK = (1<<TOIE0);
	
	
	sei();
	
	while (1)
	{
		Effect();
		AutoAdjust();
	}
}

void Effect(void)
{
	switch(StepTurn)
	{
		case 0:
		StepTurn=1;
		break;
		
		case 1:
		StepTurn=0;
		break;
		
		default:
		StepTurn=0;
	}
	return;
}

void AutoAdjust(void)
{
	
	
	return;
}