#include "lessons.h"

uint16_t lessons[LESSON_CNT * 2] EEMEM;

uint16_t read_lesson_start(uint8_t pos)
{
	return eeprom_read_word(lessons + pos);
}

uint16_t read_lesson_end(uint8_t pos)
{
	return read_lesson_start(pos + LESSON_CNT);
}

void write_lesson_start(uint8_t pos, uint16_t value)
{
	eeprom_write_word(lessons + pos, value);
}

void write_lesson_end(uint8_t pos, uint16_t value)
{
	write_lesson_start(pos + LESSON_CNT, value);
}

void write_lesson_start_end(uint8_t pos, uint16_t min)
{
	write_lesson_start(pos, min);
	min += 45;
	if (min >= 24 * 60)
		min -= 24 * 60;
	write_lesson_end(pos, min);
}

uint8_t is_last_lesson(uint8_t pos)
{
	return pos >= LESSON_CNT || read_lesson_start(pos) >= 24 * 60 || read_lesson_end(pos) >= 24 * 60;
}