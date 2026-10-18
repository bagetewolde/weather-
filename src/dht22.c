#include "../include/dht22.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>

// Adjust this to match your desired pin (e.g., PD2 or PD4)

void dht_init(void) {
DDRD |= (1 << DHT22_PIN);
PORTD |= (1 << DHT22_PIN);
}

uint8_t dht_read(uint16_t *temperature, uint16_t *humidity, uint8_t *is_negative) {
uint8_t bits[5] = {0, 0, 0, 0, 0};
uint8_t i, j;
uint16_t timeout;

// 1. Send Start Signal (Pull LOW for 2ms)
DDRD |= (1 << DHT22_PIN);
PORTD &= ~(1 << DHT22_PIN);
_delay_ms(2);

// Pull HIGH and switch to input mode
PORTD |= (1 << DHT22_PIN);
DDRD &= ~(1 << DHT22_PIN);
_delay_us(40);

// 2. Check Sensor Response
timeout = 10000;
while (PIND & (1 << DHT22_PIN)) {
if (--timeout == 0) return 0;
}

timeout = 10000;
while (!(PIND & (1 << DHT22_PIN))) {
if (--timeout == 0) return 0;
}

timeout = 10000;
while (PIND & (1 << DHT22_PIN)) {
if (--timeout == 0) return 0;
}

// 3. Read 40 Bits
for (j = 0; j < 5; j++) {
for (i = 0; i < 8; i++) {
timeout = 10000;
while (!(PIND & (1 << DHT22_PIN))) {
if (--timeout == 0) return 0;
}

_delay_us(30);

if (PIND & (1 << DHT22_PIN)) {
bits[j] |= (1 << (7 - i));
timeout = 10000;
while (PIND & (1 << DHT22_PIN)) {
if (--timeout == 0) return 0;
}
}
}
}

// 4. Verify Checksum
uint8_t checksum = bits[0] + bits[1] + bits[2] + bits[3];
if (checksum != bits[4]) {
return 0;
}

// 5. Extract Raw Values & Negative Sign Flag
*humidity = ((uint16_t)bits[0] << 8) | bits[1];

*is_negative = (bits[2] & 0x80) ? 1 : 0;
*temperature = (((uint16_t)(bits[2] & 0x7F)) << 8) | bits[3];

return 1;
}