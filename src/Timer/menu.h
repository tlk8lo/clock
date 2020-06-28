#ifndef MENU_H
#define MENU_H

#include <inttypes.h>
#include "lessons.h"

enum page
{
	NONE, MAIN, START, END
};

struct menu
{
	enum page page;
	uint8_t pos;
	uint8_t time_pos;
	uint8_t dig[4];
};

extern void btn_up(struct menu *menu);
extern void btn_right(struct menu *menu);
extern void btn_down(struct menu *menu);
extern void btn_left(struct menu *menu);

#endif /* MENU_H */