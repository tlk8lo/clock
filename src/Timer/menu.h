#ifndef MENU_H
#define MENU_H

#include <inttypes.h>
#include "lessons.h"

enum page
{
	NONE, MAIN, START, END, INIT
};

struct menu
{
	uint8_t time_pos;
	uint8_t dig[4];
	uint8_t pos;
};

extern void btn_up(enum page *page, struct menu *menu);
extern void btn_right(enum page *page, struct menu *menu, uint16_t *min);
extern void btn_down(enum page *page, struct menu *menu);
extern void btn_left(enum page *page, struct menu *menu, uint16_t *min);

#endif /* MENU_H */