#ifndef SYSTEM_H
#define SYSTEM_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "types.h"
#include "config.h"

void system_init(void);
void delay_ms(uint16_t ms);
void delay_us(uint16_t us);

#endif /* SYSTEM_H */
