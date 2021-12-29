/*
 * main.c
 *
 *  Created on: Nov 7, 2020
 *      Author: Yumna-HP
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include"lcd.h"
#include"micro_config.h"
#include"std_types.h"
#include "common_macros.h"

unsigned short on = 0;
unsigned short onandoff = 0;
unsigned short 	g_edgeCount=0;
unsigned short periodplushigh=0;

void TIMER_1()
{

	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
		TCCR1B |= (1<<CS10);
		TCNT1 = 0;
}


void INT0_init(void)
{
	DDRD  &= (~(1<<PD2));
	MCUCR = (1<<ISC01) | (1<<ISC00);
	GICR |= (1 << INT0);
	SREG = (1<<7);

}
ISR(INT0_vect)
{
	/*while(BIT_IS_SET(PORTD,6))
	  {
		  TIMSK|= (1 << TOIE1);
	  }
	  on=TCNT0;
	  CLEAR_BIT(TIMSK,TOIE1);
	  while(BIT_IS_CLEAR(PORTD,6))
	  {
		  TIMSK|= (1 << TOIE1);
	  }
	  onandoff=on+TCNT0;
	  CLEAR_BIT(TIMSK,TOIE1);*/
	g_edgeCount++;
		if(g_edgeCount == 1)
		{
			/*
			 * Clear the timer counter register to start measurements from the
			 * first detected rising edge
			 */
			TCNT1=0;
			setEdgeDetectionType(11111110);
		}
		else if(g_edgeCount == 2)
		{
			/* Store the High time value */
			on = TCNT1;
			/* Detect rising edge */
			setEdgeDetectionType(11111111);
		}
		else if(g_edgeCount == 3)
		{
			/* Store the Period time value */
			onandoff = TCNT1;
			/* Detect falling edge */
			setEdgeDetectionType(11111110);
		}
		else if(g_edgeCount == 4)
		{
			/* Store the Period time value + High time value */
			periodplushigh = TCNT1;
			/* Clear the timer counter register to start measurements again */
			TCNT1=0;
			/* Detect rising edge */
			setEdgeDetectionType(11111111);
		}
}
void setEdgeDetectionType(unsigned short pulse)
{
	/*
	 * insert the required edge bit in MCUCR Register
	 */
	MCUCR = (MCUCR & 0xFC) | (pulse);
}


void main(void)
{
	LCD_init();
	INT0_init();
	TIMER_1();
	unsigned short duty_cycle=0;

	while(1){
		if(g_edgeCount == 4)
				{
			TCCR1A = 0;
							TCCR1B = 0;
							TCNT1 = 0;
							SREG  &= ~(1<<7);
					g_edgeCount = 0;
					LCD_displayString("Duty = ");
					/* calculate the dutyCycle */
					duty_cycle = ((float)(periodplushigh-onandoff) / (periodplushigh - on)) * 100;
					/* display the dutyCycle on LCD screen */
					LCD_intgerToString(duty_cycle);
					LCD_displayCharacter('%');
				}
	}

}
