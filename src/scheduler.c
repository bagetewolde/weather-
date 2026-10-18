#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "../include/config.h"
#include "../include/scheduler.h"

static volatile uint32_t system_ticks_ms = 0;

/*
 * Timer0 CTC mode:
 * F_CPU = 16 MHz
 * Prescaler = 64
 * OCR0A = 249
 *
 * 16,000,000 / (64 * (249 + 1)) = 1000 Hz
 */

void scheduler_init(void)
{
    cli();

    TCCR0A = 0b00000010;  /* CTC mode */
    TCCR0B = 0b00000000;  /* Stop timer during setup */
    TCNT0  = 0;
    OCR0A  = 249;

    TIFR0  = (1 << OCF0A);   /* Clear pending compare flag */
    TIMSK0 = (1 << OCIE0A); /* Enable compare-A interrupt */

    TCCR0B = 0b00000011;  /* Prescaler = 64 */

    sei();
}

ISR(TIMER0_COMPA_vect)
{
    system_ticks_ms++;
}

/*
 * A 32-bit value is not read atomically on an 8-bit AVR.
 * Prevent an interrupt from changing it halfway through a read.
 */
uint32_t scheduler_now_ms(void)
{
    uint32_t result;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        result = system_ticks_ms;
    }

    return result;
}

/*
 * Returns true when a periodic task is due.
 * Call this repeatedly from the main loop.
 *
 * Do not call with period_ms == 0.
 */
bool scheduler_every(uint32_t period_ms, uint32_t *last_run_ms)
{
    uint32_t now;

    if (period_ms == 0 || last_run_ms == 0) {
        return false;
    }

    now = scheduler_now_ms();

    if ((uint32_t)(now - *last_run_ms) >= period_ms) {
        *last_run_ms = now;
        return true;
    }

    return false;
}