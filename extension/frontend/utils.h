#ifndef UTILS
#define UTILS

#include "../defs.h"

/* use this assert, since <assert.h> will stop execution - but we DON'T want that */
/* TODO: Somehow add a way to log message */
#define assert(cond)                                                                               \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      while (1)                                                                                    \
        ;                                                                                          \
    }                                                                                              \
  } while (0)

void delay_us(volatile uint count);
uint get_system_time_ms();
#define delay_ms(count) delay_us(count * 1000)
void delay_cycles(volatile int count);
void print_number(uint x);
#endif /* UTILS */
