#include "display.h"

const uint8_t font[] PROGMEM =
{
	CHAR_0,
	CHAR_1,
	CHAR_2,
	CHAR_3,
	CHAR_4,
	CHAR_5,
	CHAR_6,
	CHAR_7,
	CHAR_8,
	CHAR_9
};

void send_byte(uint8_t data)
{
	USIDR = data;
	USISR = (1<<USIOIF);
	do
		USICR = (1<<USIWM0) | (1<<USICS1) | (1<<USICLK) | (1<<USITC);
	while (!(USISR & (1<<USIOIF)));
}

void send_num_dot(uint8_t data, uint8_t dot)
{
	send_byte(pgm_read_byte(font + data) | dot);
}

void send_num(uint8_t data)
{
	send_num_dot(data, 0);
}

void display_time_difference(uint24_t sec, uint8_t sync)
{
	// uint16_t min = sec / 60;
	// Spared some bytes by using 16 bit division
	uint8_t bit = sec & 1;
	uint16_t y = sec >> 1;
	uint16_t min = y / 30;
	bit |= (y % 30) << 1;
	
	uint16_t nearest = 0xFFFF;
	uint8_t dot = 0;

	for (uint8_t i = 0; i < LESSON_CNT; i++)
	{
		uint16_t s = read_lesson_start(i);
		if (s >= 24 * 60)
			break;

		uint16_t e = read_lesson_end(i);
		if (e >= 24 * 60)
			break;

		if (s < nearest)
		{
			if (s > min)
			{
				nearest = s;
				dot = 0;
			}
			else if (s + 24 * 60 < nearest)
			{
				nearest = s + 24 * 60;
				dot = 0;
			}
		}
		if (e <= nearest)
		{
			if (e > min)
			{
				nearest = e;
				dot = CHAR_DOT;
			}
			else if (e + 24 * 60 <= nearest)
			{
				nearest = e + 24 * 60;
				dot = CHAR_DOT;
			}
		}
	}
	uint16_t minutes = nearest - min - 1;

	uint8_t m, h;
	if (minutes < 60)
	{
		// Format: mm.ss
		m = 59 - bit;
		h = minutes;
	}
	else
	{
		// Format: hh.mm
		uint16_t toDisplay = minutes < 24 * 60 ? minutes : min;

		m = toDisplay % 60;
		h = toDisplay / 60;
	}
	send_num_dot(m % 10, dot);
	send_num(m / 10);

	send_num_dot(h % 10, sync);
	uint8_t x = h / 10;
	if (x)
		send_num(x);
	else
		send_byte(0);
}

void display_time_no_sync()
{
	send_byte(CHAR_MINUS);
	send_byte(CHAR_MINUS);
	send_byte(CHAR_MINUS);
	send_byte(CHAR_MINUS);
}

void display_time(uint24_t sec, uint8_t sync)
{
	if (TIMSK1 & (1<<OCIE1A))
		display_time_difference(sec, sync);
	else
		display_time_no_sync();
}

void display_menu_pos(uint8_t pos)
{
	send_byte(CHAR_L);
	send_byte(0);
	send_num(pos % 10);
	send_num(pos / 10);
}

void display_menu_last()
{
	send_byte(CHAR_T);
	send_byte(CHAR_S);
	send_byte(CHAR_A);
	send_byte(CHAR_L);
}

void display_menu(struct menu *menu)
{
	if (menu->page == MAIN)
	{
		if (is_last_lesson(menu->pos))
			display_menu_last();
		else
			display_menu_pos(menu->pos);
	}
	else
	{
		send_num_dot(menu->dig[0], menu->time_pos == 0 ? CHAR_DOT : 0);
		send_num_dot(menu->dig[1], menu->time_pos == 1 ? CHAR_DOT : 0);
		send_num_dot(menu->dig[2], menu->time_pos == 2 ? CHAR_DOT : 0);
		send_num_dot(menu->dig[3], menu->time_pos == 3 ? CHAR_DOT : 0);
	}
}