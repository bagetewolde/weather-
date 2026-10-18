#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

void scheduler_init(void);
uint32_t scheduler_now_ms(void);
bool scheduler_every(uint32_t period_ms, uint32_t *last_run_ms);

#endif