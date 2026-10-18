#include "system.h"

void system_init(void) {
    sei();
}

void delay_ms(uint16_t ms) {
    while (ms--) {
        _delay_ms(1);
    }
}

void delay_us(uint16_t us) {
    while (us--) {
        _delay_us(1);
    }
}
