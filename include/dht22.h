#ifndef DHT22_H
#define DHT22_H
#include "config.h"
#include <stdint.h>


/* Public API Functions */
void dht_init(void);
uint8_t dht_read(uint16_t *temperature, uint16_t *humidity, uint8_t *is_negative);

#endif /* DHT22_H */
