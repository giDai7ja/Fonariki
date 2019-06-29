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

unsigned int SYS_TIK;
unsigned int TurnTime;

unsigned char StepTurn = 0;
//unsigned char TurnIndex = 0;

unsigned char d;
uint8_t i,j;

volatile uint8_t *myports[] = {
	&PORTD,
	&PORTD,
	&PORTC,
	&PORTC,
	&PORTC,
	&PORTC,
	&PORTC,
	&PORTC,
	&PORTD,
	&PORTD,
	&PORTD,
	&PORTD,
	&PORTD,
	&PORTB,
	&PORTB,
	&PORTB
};
volatile uint8_t pin[] = {
	PD1,
	PD0,
	PC5,
	PC4,
	PC3,
	PC2,
	PC1,
	PC0,
	PD3,
	PD4,
	PD5,
	PD6,
	PD7,
	PB0,
	PB1,
	PB2
};

//int main() {
//uint8_t i,j;
//for (i=0; i < 3; i++) {
//*(myports[i]) |= 0x10;  // set this bit in each port
//}
//}

//const char turn[64] = {
//0b00000000, 0b00000001,
//0b00000000, 0b00000011,
//0b00000000, 0b00000111,
//0b00000000, 0b00001111,
//0b00000000, 0b00011111,
//0b00000000, 0b00111111,
//0b00000000, 0b01111111,
//0b00000000, 0b11111111,
//0b00000001, 0b11111111,
//0b00000011, 0b11111111,
//0b00000111, 0b11111111,
//0b00001111, 0b11111111,
//0b00011111, 0b11111111,
//0b00111111, 0b11111111,
//0b01111111, 0b11111111,
//0b11111111, 0b11111111,
//0b11111111, 0b11111110,
//0b11111111, 0b11111100,
//0b11111111, 0b11111000,
//0b11111111, 0b11110000,
//0b11111111, 0b11100000,
//0b11111111, 0b11000000,
//0b11111111, 0b10000000,
//0b11111111, 0b00000000,
//0b11111110, 0b00000000,
//0b11111100, 0b00000000,
//0b11111000, 0b00000000,
//0b11110000, 0b00000000,
//0b11100000, 0b00000000,
//0b11000000, 0b00000000,
//0b10000000, 0b00000000,
//0b00000000, 0b00000000
//};

ISR(TIMER0_OVF_vect){
	SYS_TIK++;
}

void Init();
void Effect(void);
void AutoAdjust(void);


int main(void)
{
	Init();
	while (1)
	{
		//		for (i=0; i < 3; i++) {
		//			*(myports[i]) |= (1<<pin[i]);  // set this bit in each port
		//		}
		Effect();
		//		AutoAdjust();
	}
}

void Init()
{
	d = eeprom_read_byte(&D1);
	d = 90;
	PORTB = 0x00;
	PORTD = 0x00;
	PORTC = 0x00;
	DDRB = 0x07;  // PORTB as OUTPUT
	DDRD = 0xFB;  // PORTD as OUTPUT
	DDRC = 0x3F;  // PORTC as OUTPUT

	TCCR0 = (1<<CS01); // CLK/8
	//	TCCR0 = (1<<CS00)|(1<<CS01); // CLK/64
	//	TCCR0 = (1<<CS00)|(1<<CS02); // CLK/1024
	TIMSK = (1<<TOIE0);
	
	sei();
}

void Effect(void)
{
	if (TurnTime < SYS_TIK) {
		switch(StepTurn)
		{
			case 0:
			if ( i == 0 ) {
				SYS_TIK=0;
				TurnTime=0;
			}
			*(myports[i]) |= (1<<pin[i]);
			i++;
			if ( i == 16 ) {
				StepTurn=10;
				i=0;
			}
			break;

			case 10:
			if ( i < 16 ) *(myports[i]) &= ~(1<<pin[i]);
			if ( i > 3 ) *(myports[i-4]) |= (1<<pin[i-4]);
			i++;
			if ( i == 20 ) {
				StepTurn=40;
				i=0;
			}
			break;
			
			case 40:
			*(myports[i]) &= ~(1<<pin[i]);
			i++;
			if ( i == 16 ) {
				StepTurn=0;
				i=0;
				TurnTime+=d*31;
			}
			break;
			
			default:
			StepTurn=100;
		}
		TurnTime+=d;
	}
}

void AutoAdjust(void)
{
	
	return;
}