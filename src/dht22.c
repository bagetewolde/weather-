#include "../include/dht22.h"
#include <avr/interrupt.h>

/* Pointer references for the chosen GPIO pin */
static volatile uint8_t *dht_port = NULL;
static volatile uint8_t *dht_ddr = NULL;
static volatile uint8_t *dht_pin = NULL;
static uint8_t dht_bit = 0;

void dht22_init(volatile uint8_t *port, volatile uint8_t *ddr, volatile uint8_t *pin, uint8_t bit) {
    dht_port = port;
    dht_ddr = ddr;
    dht_pin = pin;
    dht_bit = bit;

    // Set pin high initially with pull-up enabled / input state
    *dht_ddr &= ~(1 << dht_bit);
    *dht_port |= (1 << dht_bit);
}

status_t dht22_read(dht22_data_t *data) {
    if (!dht_port || !dht_ddr || !dht_pin || !data) {
        return STATUS_ERROR;
    }

    uint8_t bits[5] = {0, 0, 0, 0, 0};
    uint8_t i, j;

    /* --- Step 1: Send Start Signal --- */
    // Pull line LOW for at least 1ms (1000us) to trigger DHT22 wakeup
    *dht_ddr |= (1 << dht_bit);   // Set as OUTPUT
    *dht_port &= ~(1 << dht_bit); // Drive LOW
    _delay_ms(2);                 

    // Release line (set as INPUT with pull-up) and wait for response
    *dht_ddr &= ~(1 << dht_bit);
    *dht_port |= (1 << dht_bit);

    // Disable interrupts temporarily to ensure exact microsecond timing precision
    cli();

    /* --- Step 2: Check Sensor Response Signal --- */
    // Wait for sensor to pull line LOW (response signal start, takes ~20-40us)
    uint16_t timeout = 0;
    while ((*dht_pin & (1 << dht_bit))) {
        if (++timeout > 1000) {
            sei();
            return STATUS_ERROR; // Timeout waiting for low
        }
        _delay_us(1);
    }

    // Wait for sensor to pull line HIGH (~80us response pulse)
    timeout = 0;
    while (!(*dht_pin & (1 << dht_bit))) {
        if (++timeout > 1000) {
            sei();
            return STATUS_ERROR;
        }
        _delay_us(1);
    }

    // Wait for sensor to pull line LOW again (start of data transmission)
    timeout = 0;
    while ((*dht_pin & (1 << dht_bit))) {
        if (++timeout > 1000) {
            sei();
            return STATUS_ERROR;
        }
        _delay_us(1);
    }

    /* --- Step 3: Read 40 Bits of Data (5 Bytes) --- */
    for (j = 0; j < 5; j++) {
        for (i = 0; i < 8; i++) {
            // Wait for the pin to go HIGH (start of a bit pulse)
            timeout = 0;
            while (!(*dht_pin & (1 << dht_bit))) {
                if (++timeout > 1000) {
                    sei();
                    return STATUS_ERROR;
                }
                _delay_us(1);
            }

            // Measure how long the pin stays HIGH:
            // A ~26-28us high pulse means a binary '0'
            // A ~70us high pulse means a binary '1'
            uint8_t high_duration = 0;
            while ((*dht_pin & (1 << dht_bit))) {
                high_duration++;
                if (high_duration > 100) break;
                _delay_us(1);
            }

            // Shift bits into our byte buffer (MSB first)
            bits[j] <<= 1;
            if (high_duration > 40) { // Threshold between 28us and 70us
                bits[j] |= 1;
            }
        }
    }

    // Re-enable interrupts
    sei();

    /* --- Step 4: Verify Checksum --- */
    uint8_t checksum = bits[0] + bits[1] + bits[2] + bits[3];
    if (checksum != bits[4]) {
        return STATUS_ERROR; // Checksum mismatch
    }

    /* --- Step 5: Convert Raw Data into Real Values --- */
    // Humidity (16-bit)
    uint16_t raw_hum = ((uint16_t)bits[0] << 8) | bits[1];
    data->humidity = (float)raw_hum / 10.0f;

    // Temperature (16-bit, MSB bit 7 is negative sign)
    uint16_t raw_temp = ((uint16_t)(bits[2] & 0x7F) << 8) | bits[3];
    data->temperature = (float)raw_temp / 10.0f;
    if (bits[2] & 0x80) {
        data->temperature = -data->temperature;
    }

    return STATUS_OK;
}
