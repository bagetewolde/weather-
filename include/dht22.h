#ifndef DHT22_H
#define DHT22_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "../include/system.h"

/* Structure to hold sensor readings */
typedef struct {
    float temperature;
    float humidity;
} dht22_data_t;

/* Public API Functions */
void dht22_init(volatile uint8_t *port, volatile uint8_t *ddr, volatile uint8_t *pin, uint8_t bit);
status_t dht22_read(dht22_data_t *data);

#endif /* DHT22_H */
