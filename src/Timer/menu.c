#include "menu.h"

static uint16_t digits_to_minutes(uint8_t *dig)
{
	uint8_t hours = dig[3] * 10 + dig[2];

	uint8_t minutes = dig[1] * 10 + dig[0];

	return (uint16_t)hours * 60 + minutes;
}

static void minutes_to_digits(uint8_t *dig, uint16_t min)
{
	uint8_t minutes = min % 60;
	dig[0] = minutes % 10;
	dig[1] = minutes / 10;

	uint8_t hours = min / 60;
	dig[2] = hours % 10;
	dig[3] = hours / 10;
}

static uint8_t inc(uint8_t dig, uint8_t max)
{
	dig++;
	if (dig > max)
		return 0;
	return dig;
}

static uint8_t dec(uint8_t dig, uint8_t max)
{
	dig--;
	if (dig == 0xFF)
		return max;
	return dig;
}

static void write_minutes(enum page *page, uint8_t pos, uint16_t min)
{
	if (*page == INIT)
		*page = NONE;
	else
	{
		if (*page == START)
			write_lesson_start_end(pos, min);
		else
			write_lesson_end(pos, min);
		*page = MAIN;
	}
}

void btn_up(enum page *page, struct menu *menu)
{
	if (*page == MAIN)
	{
		if (is_last_lesson(menu->pos))
		{
			if (menu->pos < LESSON_CNT)
			{
				if (menu->pos)
				{
					uint16_t min = read_lesson_end(menu->pos - 1) + 10;
					if (min >= 24 * 60)
						min -= 24 * 60;
					write_lesson_start_end(menu->pos, min);
				}
				else
					write_lesson_start_end(0, 8 * 60);
			}
		}
		else
		{
			*page = START;
			menu->time_pos = 3;
			minutes_to_digits(menu->dig, read_lesson_start(menu->pos));
		}
	}
	else
	{
		switch (menu->time_pos)
		{
			case 0:  menu->dig[0] = inc(menu->dig[0], 9); break;
			case 1:  menu->dig[1] = inc(menu->dig[1], 5); break;
			case 2:  menu->dig[2] = inc(menu->dig[2], 9); break;
			default: menu->dig[3] = inc(menu->dig[3], 2); break;
		}

		if (menu->dig[3] == 2 && menu->dig[2] >= 4)
			menu->dig[2] = 0;
	}
}

void btn_right(enum page *page, struct menu *menu, uint16_t *min)
{
	if (*page == MAIN)
	{
		(menu->pos)--;
		if (menu->pos == 0xFF)
			*page = NONE;
	}
	else
	{
		(menu->time_pos)++;
		if (menu->time_pos > 3)
		{
			*min = digits_to_minutes(menu->dig);
			write_minutes(page, menu->pos, *min);
		}
	}
}

void btn_down(enum page *page, struct menu *menu)
{
	if (*page == MAIN)
	{
		if (is_last_lesson(menu->pos))
		{
			if (menu->pos)
			{
				(menu->pos)--;
				write_lesson_start(menu->pos, 0xFFFF);
				write_lesson_end(menu->pos, 0xFFFF);
				if (menu->pos)
					(menu->pos)--;
			}
		}
		else
		{
			*page = END;
			menu->time_pos = 3;
			minutes_to_digits(menu->dig, read_lesson_end(menu->pos));
		}
	}
	else
	{
		switch(menu->time_pos)
		{
			case 0:  menu->dig[0] = dec(menu->dig[0], 9); break;
			case 1:  menu->dig[1] = dec(menu->dig[1], 5); break;
			case 2:  menu->dig[2] = dec(menu->dig[2], menu->dig[3] == 2 ? 3 : 9); break;
			default:
				(menu->dig[3])--;
				if (menu->dig[3] == 0xFF)
				{
					menu->dig[3] = 2;
					if (menu->dig[2] >= 4)
						menu->dig[2] = 0;
				}
				break;
		}
	}
}

void btn_left(enum page *page, struct menu *menu, uint16_t *min)
{
	if (*page == MAIN)
	{
		if (is_last_lesson(menu->pos))
			*page = NONE;
		else
			(menu->pos)++;
	}
	else
	{
		(menu->time_pos)--;
		if (menu->time_pos == 0xFF)
		{
			*min = digits_to_minutes(menu->dig);
			write_minutes(page, menu->pos, *min);
		}
	}
}