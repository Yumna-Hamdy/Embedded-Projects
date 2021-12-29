/*
 * src.c
 *
 *  Created on: Oct 23, 2020
 *      Author: Yumna-HP
 */

/*
 * project1.c
 *
 *  Created on: Oct 21, 2020
 *      Author: Yumna-HP
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned int minutes = 0;
unsigned int seconds = 0;
unsigned int hours = 0;

void INT0_init(void);
void INT1_init(void);
void INT2_init(void);

void TIMER_1()
{

	TCNT1 = 0;
	OCR1A = 1000;
	TCCR1A = (1 << FOC1A) | (1 << FOC1B);
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	TIMSK |= (1 << OCIE1A);
	SREG |= (1 << 7);
}

int main(void)
{
	DDRD &= ~(1 << PD2);
	DDRD &= ~(1 << PD3);
	PORTD |= (1 << PD2);
	DDRA |= 0X3F;
	//PORTA&= 0xF0;
	DDRC = 0X0F;
	SREG |= (1 << 7);
	TIMER_1();
	INT0_init();
	INT1_init();
	INT2_init();
	while (1)
	{
		PORTA &= 0;

		PORTA = (1 << 5);
		PORTC = seconds % 10;
		_delay_ms(5);

		PORTA = (1 << 4);
		PORTC = (seconds / 10);
		_delay_ms(5);

		PORTA = (1 << 3);
		PORTC = minutes % 10;
		_delay_ms(5);

		PORTA = (1 << 2);
		PORTC = (minutes / 10);
		_delay_ms(5);

		PORTA = (1 << 1);
		PORTC = hours % 10;
		_delay_ms(5);

		PORTA = (1 << 0);
		PORTC = hours / 10;
		_delay_ms(5);

	}
}

void INT0_init(void)
{
	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1 << ISC00);
	GICR |= (1 << INT0);
}

void INT1_init(void)
{
	MCUCR |= (1 << ISC11) | (1 < ISC10);
	GICR |= (1 << INT1);
}

void INT2_init(void)
{
	MCUCSR &= ~(1 << ISC2);
	GICR |= (1 << INT2);
}

ISR(TIMER1_COMPA_vect)
{
	seconds++;
	if (seconds == 60)
	{
		seconds = 0;
		minutes++;
	}
	if (minutes == 60)
	{
		minutes = 0;
		hours++;
	}
}

ISR(INT0_vect)
{
	seconds = 0;
	minutes = 0;
	hours = 0;
}

ISR(INT1_vect)
{
	TCCR1B &= ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10);
}

ISR(INT2_vect)
{
	TCCR1B |= (1 << CS12) | (1 << CS10);
	TCCR1B &= ~(1 << CS11);
}
