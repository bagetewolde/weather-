#include "../include/i2c.h"
#include <avr/io.h>
#include <util/twi.h>

void twi_init(void) {
    TWSR = 0x00; /* Prescaler = 1 */
    TWBR = (uint8_t)(((F_CPU / I2C_SCL_HZ) - 16) / 2);
    TWCR = (1 << TWEN);
}

status_t twi_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    uint8_t status = TW_STATUS;
    if (status != TW_START && status != TW_REP_START) {
        return STATUS_ERROR;
    }
    return STATUS_OK;
}

void twi_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}

status_t twi_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    uint8_t status = TW_STATUS;
    if (status != TW_MT_SLA_ACK && status != TW_MT_DATA_ACK && status != TW_MR_SLA_ACK) {
        return STATUS_ERROR;
    }
    return STATUS_OK;
}

uint8_t twi_read_ack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

uint8_t twi_read_nack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

status_t twi_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
    if (twi_start() != STATUS_OK) return STATUS_ERROR;
    if (twi_write((dev_addr << 1) | TWI_WRITE) != STATUS_OK) {
        twi_stop();
        return STATUS_ERROR;
    }
    if (twi_write(reg_addr) != STATUS_OK) {
        twi_stop();
        return STATUS_ERROR;
    }
    if (twi_write(data) != STATUS_OK) {
        twi_stop();
        return STATUS_ERROR;
    }
    twi_stop();
    return STATUS_OK;
}

status_t twi_read_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t length) {
    if (!buffer || length == 0) return STATUS_ERROR;

    if (twi_start() != STATUS_OK) return STATUS_ERROR;
    if (twi_write((dev_addr << 1) | TWI_WRITE) != STATUS_OK) {
        twi_stop();
        return STATUS_ERROR;
    }
    if (twi_write(reg_addr) != STATUS_OK) {
        twi_stop();
        return STATUS_ERROR;
    }

    if (twi_start() != STATUS_OK) return STATUS_ERROR;
    if (twi_write((dev_addr << 1) | TWI_READ) != STATUS_OK) {
        twi_stop();
        return STATUS_ERROR;
    }

    for (uint8_t i = 0; i < length; i++) {
        if (i == length - 1) {
            buffer[i] = twi_read_nack();
        } else {
            buffer[i] = twi_read_ack();
        }
    }

    twi_stop();
    return STATUS_OK;
}
