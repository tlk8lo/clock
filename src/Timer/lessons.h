#ifndef LESSONS_H
#define LESSONS_H

#include <avr/eeprom.h>

#define LESSON_CNT	32

extern uint16_t read_lesson_start(uint8_t pos);
extern uint16_t read_lesson_end(uint8_t pos);
extern void write_lesson_start(uint8_t pos, uint16_t value);
extern void write_lesson_end(uint8_t pos, uint16_t value);
extern void write_lesson_start_end(uint8_t pos, uint16_t min);
extern uint8_t is_last_lesson(uint8_t pos);

#endif /* LESSONS_H */