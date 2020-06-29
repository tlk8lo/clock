/*
	A school timer which displays time to the end of lesson/break.
*/ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "dcf.h"
#include "display.h"
#include "menu.h"

/*
	Ports configuration
*/
#define DCF_DDR		DDRA
#define DCF_PORT	PORTA
#define DCF_PIN		PINA
#define DCF_BIT		7

#define BTN_DDR		DDRA
#define BTN_PORT	PORTA
#define BTN_PIN		PINA
#define BTN_MASK	15
#define BTN_U		0
#define BTN_R		1
#define BTN_D		2
#define BTN_L		3

#define DISP_DDR	DDRA
#define DISP_PORT	PORTA
#define DISP_PIN	PINA
#define DISP_CLK	4
#define DISP_DATA	5
#define DISP_LOAD	6

/*
	Timer1 comp interrupt period consts

	Interrupt period = 1 s * F_CPU / Prescaler
	Interrupt period = 1 * 8000000 / 256
	Interrupt period = 31250
*/
#define SEC_OCR_VAL	31250
#define SEC_RES_VAL	6250
#define BTN_DEB_VAL	312

/*
	Timer0 comp interrupt period consts

	Interrupt period = 20 ms * F_CPU / Prescaler
	Interrupt period = 0.02 * 8000000 / 1024
	Interrupt period = 156
*/
#define DCF_OCR_VAL	156

/*
	DCF waveform

	Time:          0        100       200       300       400       500       600       700       800       900       1000
	               ---------------------                                                                               ------
	               |         |         |                                                                               |
	Waveform:      |   "0"   |   "1"   |                                                                               |
	               |         |         |                                                                               |
	           -----         -------------------------------------------------------------------------------------------
	               ^    ^         ^         ^                                                                     ^         ^
	Threshold:   Reset  1         2         3                                                                     4         5
*/

/*
	DCF time consts (in units of 20 ms)
	1 - 40 ms
	2 - 140 ms
	3 - 240 ms
	4 - 900 ms
	5 - 1100 ms
	OVF - 1800 ms
	RES - 200 ms
*/
#define DCF_1_THRES	2
#define DCF_2_THRES	7
#define DCF_3_THRES	12
#define DCF_4_THRES	45
#define DCF_5_THRES	55
#define DCF_OVF_VAL	90
#define DCF_RES_VAL	10

#define UPDATE_DISP	0
#define SYNC		1
//static volatile uint8_t flags;
#define flags		GPIOR0

//static volatile uint8_t ms_dcf;
#define ms_dcf		GPIOR1
static uint8_t data[59];

static volatile uint24_t seconds;
static volatile uint24_t last_sync;
//static volatile uint8_t sync_dot;
#define sync_dot	GPIOR2

struct dcf
{
	uint8_t state;
	uint8_t cnt;
};

static void check_dcf(struct dcf *dcf)
{
	cli();
	uint8_t tim = ms_dcf;

	// DCF timer overflow
	if (tim == DCF_OVF_VAL)
	{
		sei();
		if (dcf->cnt == 59 && !dcf->state && dcf_validate(data))
		{
			uint24_t s = dcf_parse(data);
			
			cli();
			uint16_t cnt = TCNT1;
			GTCCR = (1<<PSR10);
			if (cnt >= SEC_OCR_VAL - (SEC_RES_VAL - 1))
				OCR1A = cnt + (SEC_RES_VAL - 1) - SEC_OCR_VAL;
			else
				OCR1A = cnt + (SEC_RES_VAL - 1);
			TIFR1 = (1<<OCF1A);

			last_sync = s;
			sync_dot = CHAR_DOT;
			flags |= (1<<SYNC);
			TIMSK1 = (1<<OCIE1A);
		}

		dcf->cnt = 0;
	}
	
	uint8_t new_state = DCF_PIN & (1<<DCF_BIT);
	sei();

	// DCF edge interrupt
	if (new_state != dcf->state)
	{
		// Rising edge
		if (new_state)
		{
			cli();
			TCNT0 = 0;
			TIFR0 = (1<<OCF0A);
			ms_dcf = 0;
			sei();

			if ((dcf->cnt && dcf->cnt < 59 && tim >= DCF_4_THRES && tim < DCF_5_THRES) || tim == DCF_OVF_VAL)
				dcf->cnt++;
			else
				dcf->cnt = 0;
		}
		// Falling edge
		else
		{
			if (dcf->cnt && tim >= DCF_1_THRES && tim < DCF_3_THRES)
				data[dcf->cnt - 1] = tim >= DCF_2_THRES;
			else
			{
				cli();
				TCNT0 = 0;
				TIFR0 = (1<<OCF0A);
				ms_dcf = DCF_RES_VAL;
				sei();
				
				dcf->cnt = 0;
			}
		}
		
		dcf->state = new_state;
	}
}

static void check_pin_change()
{
	cli();
	if (GIFR & (1<<PCIF0))
	{
		GIFR = (1<<PCIF0);
		uint16_t cnt = TCNT1;
		if (cnt >= SEC_OCR_VAL - (BTN_DEB_VAL - 1))
			OCR1B = cnt + (BTN_DEB_VAL - 1) - SEC_OCR_VAL;
		else
			OCR1B = cnt + (BTN_DEB_VAL - 1);
		TIFR1 = (1<<OCF1B);
	}
	sei();
}

static inline void load_display()
{
	DISP_PORT |= (1<<DISP_LOAD);
	DISP_PORT &= ~(1<<DISP_LOAD);
}

/*
	Increment DCF timer until it reaches DCF_OVF_VAL
*/
ISR(TIM0_COMPA_vect)
{
	uint8_t dcf = ms_dcf;
	if (dcf < DCF_OVF_VAL)
		ms_dcf = dcf + 1;
}

/*
	Increment seconds counter
*/
ISR(TIM1_COMPA_vect)
{
	uint24_t s = seconds + 1;
	if (s >= (uint24_t)24 * 60 * 60)
		s = 0;

	uint24_t l = last_sync;
	if (flags & (1<<SYNC))
	{
		flags &= ~(1<<SYNC);
		s = l;
	}
	else if (s == l)
		sync_dot = 0;
	
	seconds = s;
	flags |= (1<<UPDATE_DISP);
}

int main(void)
{
	// Setup ports
	DDRA = (1<<DISP_CLK) | (1<<DISP_DATA) | (1<<DISP_LOAD);
	PORTA = (1<<BTN_U) | (1<<BTN_R) | (1<<BTN_D) | (1<<BTN_L) | (1<<DCF_BIT);
	
	// Turn off ADC
	PRR |= (1<<PRADC);

	// Pin Change Interrupt config
	PCMSK0 = (1<<PCINT0) | (1<<PCINT1) | (1<<PCINT2) | (1<<PCINT3);
	
	// USI config
	USICR = (1<<USIWM0) | (1<<USICS1) | (1<<USICLK);

	// Timer 0 config - CTC, prescaler = 1024
	OCR0A = DCF_OCR_VAL - 1;
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS02) | (1<<CS00);
	TIMSK0 = (1<<OCIE0A);

	// Timer 1 config - CTC, prescaler = 256
	ICR1 = SEC_OCR_VAL - 1;
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS12);

	flags |= (1<<UPDATE_DISP);

	uint8_t old = BTN_PIN;
	struct dcf dcf = { old & (1<<DCF_BIT) };
	
	// Main loop
	while (1)
	{
		sei();
		struct menu menu = { 0 };
		
		do
		{
			check_dcf(&dcf);
			check_pin_change();

			if (TIFR1 & (1<<OCF1B))
			{
				uint8_t new = BTN_PIN;
				uint8_t btn = old & ~new;
				old = new;
				
				if (btn & BTN_MASK)
				{
					if (menu.page == NONE)
					{
						menu.page = MAIN;
						flags |= (1<<UPDATE_DISP);
					}
					else if (btn & (1<<BTN_U))
						btn_up(&menu);
					else if (btn & (1<<BTN_R))
						btn_right(&menu);
					else if (btn & (1<<BTN_D))
						btn_down(&menu);
					else if (btn & (1<<BTN_L))
						btn_left(&menu);
				}

				if (menu.page != NONE)
				{
					display_menu(&menu);
					load_display();
				}
			}
		} while (menu.page != NONE);
		
		cli();
		if (flags & (1<<UPDATE_DISP))
		{
			flags &= ~(1<<UPDATE_DISP);
			uint24_t sec = seconds;
			uint8_t sync = sync_dot;
			sei();

			display_time(sec, sync);
			load_display();
		}
	}
}
