#ifndef TWI_H
#define TWI_H

#include "types.h"
#include "config.h"

#define TWI_WRITE   0
#define TWI_READ    1

void twi_init(void);
status_t twi_start(void);
void twi_stop(void);
status_t twi_write(uint8_t data);
uint8_t twi_read_ack(void);
uint8_t twi_read_nack(void);

status_t twi_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
status_t twi_read_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t length);

#endif /* TWI_H */
