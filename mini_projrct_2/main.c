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
#include"adc.h"
#include"micro_config.h"
#include"std_types.h"
#include "common_macros.h"

ISR(INT1_vect)
{
	PORTC^= (1<<PC0);
	PORTC ^=(1<<PC1);
}
void INT1_Init()
{
	DDRD&=(~(1<<PD3));
	MCUCR |= (1 << ISC11);
	MCUCR |= (1 << ISC10);
	GICR |= (1 << INT1);

}

void PWM_Timer0_Init(unsigned char set_duty_cycle)
{

	TCNT0 = 0; //Set Timer Initial value

	OCR0  = set_duty_cycle; // Set Compare Value

	DDRB  = DDRB | (1<<PB3); //set PB3/OC0 as output pin --> pin where the PWM signal is generated from MC.

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
}


void main(void)
{
	uint16 res_value;
	CLEAR_BIT(DDRA,0);
	CLEAR_BIT(DDRD,2);//input
	DDRC  |= 0x0F;   //output
	DDRB|= 0x03;
	PORTB &= 0xFC;
	SREG |= (1 << 7);
	INT1_Init();


		LCD_init(); /* initialize LCD driver */
		ADC_init(); /* initialize ADC driver */
		LCD_clearScreen(); /* clear LCD at the beginning */
		/* display this string "ADC Value = " only once at LCD */
		LCD_displayString("ADC Value = ");
	    while(1)
	    {
			LCD_goToRowColumn(0,12); /* display the number every time at this position */
			res_value = ADC_readChannel(0); /* read channel zero where the potentiometer is connect */
			LCD_intgerToString(res_value); /* display the ADC value on LCD screen */
			PWM_Timer0_Init(res_value/4);

	    }

	LCD_displayStringRowColumn(0,2,"ADC value =");

}
