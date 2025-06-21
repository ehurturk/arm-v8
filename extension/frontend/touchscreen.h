#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <stdint.h>
#include <stdbool.h>
#include "../defs.h"

void touchscreen_init(void);

uint16_t touchscreen_read_z1(void);

bool touchscreen_get_point(u16 *x, u16 *y);

bool touchscreen_pen_down(void);

#endif // TOUCHSCREEN_H