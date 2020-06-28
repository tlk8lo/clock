#ifndef DISPLAY_H
#define DISPLAY_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lessons.h"
#include "menu.h"

typedef __uint24 uint24_t;

#define CHAR_0		0x3F
#define CHAR_1		0x06
#define CHAR_2		0x5B
#define CHAR_3		0x4F
#define CHAR_4		0x66
#define CHAR_5		0x6D
#define CHAR_6		0x7D
#define CHAR_7		0x07
#define CHAR_8		0x7F
#define CHAR_9		0x6F
#define CHAR_L		0x38
#define CHAR_A		0x77
#define CHAR_S		0x6D
#define CHAR_T		0x78
#define CHAR_MINUS	0x40
#define CHAR_DOT	0x80

extern const uint8_t font[10] PROGMEM;

extern void display_time(uint24_t sec, uint8_t sync);
extern void display_menu(struct menu *menu);

#endif /* DISPLAY_H */
