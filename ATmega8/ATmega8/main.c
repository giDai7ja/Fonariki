/*
* ATmega8.c
*
* Created: 24.11.2018 13:25:45
* Author : Peppa
*/

#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

//#define F_CPU 8000000UL
unsigned char D1 EEMEM;

unsigned long int SYS_TIK=0;
unsigned long int NextStepTime=0;
unsigned long int StartTime=0, StopTime=0, SaveTime=0;

unsigned char StepTurn = 0;

unsigned char d, d_eeprom;
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
		while ( !(PIND & (1<<PIND2)) || ((StartTime+400)>SYS_TIK) )
		{
			Effect();
		}
		PORTB = 0;
		PORTD = 0;
		PORTC = 0;
		DDRB = 0;  // PORTB as INTPUT
		DDRD = 0;  // PORTD as INTPUT
		DDRC = 0;  // PORTC as INTPUT
		StopTime=SYS_TIK;
		StepTurn=0;
		i=0;
		AutoAdjust();
		SaveTime=StopTime+11718;
		while ( (PIND & (1<<PIND2)) )
		{
			if ( (SaveTime<SYS_TIK) && (d_eeprom != d ) ) {
				eeprom_update_byte(&D1, d);
				d_eeprom=d;
				//				DDRB = 0x07;
				//				PORTB = 0x07;
			}
			_NOP();
		}
		
	}
}

void Init()
{
	d_eeprom = eeprom_read_byte(&D1);
	d = d_eeprom;

	TCCR0 = (1<<CS01); // CLK/8
	TIMSK = (1<<TOIE0);
	
	sei();
}

void Effect(void)
{
	if (NextStepTime <= SYS_TIK) {
		switch(StepTurn)
		{
			case 0:
			if ( i == 0 ) {
				NextStepTime=SYS_TIK;
				StartTime=SYS_TIK;
				StopTime=SYS_TIK;
				PORTB = 0;
				PORTD = 0;
				PORTC = 0;
				DDRB = 0x07;  // PORTB as OUTPUT
				DDRD = 0xFB;  // PORTD as OUTPUT
				DDRC = 0x3F;  // PORTC as OUTPUT
			}
			*(myports[i]) |= (1<<pin[i]);
			i++;
			if ( i == 16 ) {
				StepTurn=40;
				NextStepTime+=d*8;
				i=0;
			}
			break;

			case 10:
			if ( i < 16 ) *(myports[i]) &= ~(1<<pin[i]);
			if ( i > 2 ) *(myports[i-3]) |= (1<<pin[i-3]);
			i++;
			if ( i == 19 ) {
				StepTurn=20;
				i=0;
			}
			break;

			case 20:
			if ( i < 16 ) *(myports[i]) &= ~(1<<pin[i]);
			if ( i > 2 ) *(myports[i-3]) |= (1<<pin[i-3]);
			i++;
			if ( i == 19 ) {
				StepTurn=40;
				i=0;
			}
			break;
			
			case 40:
			*(myports[i]) &= ~(1<<pin[i]);
			i++;
			if ( i == 16 ) {
				StepTurn=100;
				i=0;
			}
			break;
			
			default:
			StepTurn=100;
		}
		NextStepTime+=d;
	}
}

void AutoAdjust(void)
{
	unsigned long int Time;
	Time=(StopTime-StartTime);
	if ( (Time > 976) && (Time<7812) )d=Time/40;
}