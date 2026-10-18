#include "../include/bmp280.h"
#include <util/delay.h>

/* --- Hardware Chip Select Pin Definition --- */

#define BMP280_CS_PORT PORTB
#define BMP280_CS_DDR DDRB
#define BMP280_CS_PIN PB2 /* Change this if your CS pin is on a different pin */

/* Initialize AVR Hardware SPI Master & CS Pin */
static void spi_hardware_init(void) {
// Configure SPI pins on standard AVR (e.g., ATmega328P):
// MOSI (PB3), SCK (PB5) as OUTPUT. MISO (PB4) as INPUT.
// Also set CS pin as OUTPUT.
DDRB |= (1 << PB3) | (1 << PB5) | (1 << BMP280_CS_PIN);
DDRB &= ~(1 << PB4);

// Deselect sensor initially (CS HIGH)
BMP280_CS_PORT |= (1 << BMP280_CS_PIN);

/* Enable SPI, Master mode, Clock rate f_osc/16, SPI Mode 0 (CPOL=0, CPHA=0) */
SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

/* Transmit/Receive a single byte via AVR Hardware SPI Register */
static uint8_t spi_transfer_byte(uint8_t out_byte) {
SPDR = out_byte;
while (!(SPSR & (1 << SPIF))); // Wait for transmission complete
return SPDR;
}

static status_t bmp280_spi_write_byte(uint8_t reg_addr, uint8_t data) {
BMP280_CS_PORT &= ~(1 << BMP280_CS_PIN); // Pull CS LOW

// BMP280 SPI Write: MSB of address must be 0
spi_transfer_byte(reg_addr & 0x7F);
spi_transfer_byte(data);

BMP280_CS_PORT |= (1 << BMP280_CS_PIN); // Pull CS HIGH
return STATUS_OK;
}

static status_t bmp280_spi_read_bytes(uint8_t reg_addr, uint8_t *buffer, uint8_t length) {
if (!buffer || length == 0) return STATUS_ERROR;

BMP280_CS_PORT &= ~(1 << BMP280_CS_PIN); // Pull CS LOW

// BMP280 SPI Read: MSB of address must be 1
spi_transfer_byte(reg_addr | 0x80);

for (uint8_t i = 0; i < length; i++) {
buffer[i] = spi_transfer_byte(0x00); // Send dummy byte to clock in data
}

BMP280_CS_PORT |= (1 << BMP280_CS_PIN); // Pull CS HIGH
return STATUS_OK;
}

static status_t bmp280_get_calib_data(bmp280_dev_t *dev) {
uint8_t calib_buf[24];

if (bmp280_spi_read_bytes(BMP280_REG_CALIB_START, calib_buf, 24) != STATUS_OK) {
return STATUS_ERROR;
}

dev->calib.dig_T1 = (uint16_t)(((uint16_t)calib_buf[1] << 8) | calib_buf[0]);
dev->calib.dig_T2 = (int16_t)(((int16_t)calib_buf[3] << 8) | calib_buf[2]);
dev->calib.dig_T3 = (int16_t)(((int16_t)calib_buf[5] << 8) | calib_buf[4]);

dev->calib.dig_P1 = (uint16_t)(((uint16_t)calib_buf[7] << 8) | calib_buf[6]);
dev->calib.dig_P2 = (int16_t)(((int16_t)calib_buf[9] << 8) | calib_buf[8]);
dev->calib.dig_P3 = (int16_t)(((int16_t)calib_buf[11] << 8) | calib_buf[10]);
dev->calib.dig_P4 = (int16_t)(((int16_t)calib_buf[13] << 8) | calib_buf[12]);
dev->calib.dig_P5 = (int16_t)(((int16_t)calib_buf[15] << 8) | calib_buf[14]);
dev->calib.dig_P6 = (int16_t)(((int16_t)calib_buf[17] << 8) | calib_buf[16]);
dev->calib.dig_P7 = (int16_t)(((int16_t)calib_buf[19] << 8) | calib_buf[18]);
dev->calib.dig_P8 = (int16_t)(((int16_t)calib_buf[21] << 8) | calib_buf[20]);
dev->calib.dig_P9 = (int16_t)(((int16_t)calib_buf[23] << 8) | calib_buf[22]);

return STATUS_OK;
}

status_t bmp280_init(bmp280_dev_t *dev) {
if (!dev) return STATUS_ERROR;

spi_hardware_init();
_delay_ms(10);

// Verify chip ID
uint8_t chip_id = 0;
if (bmp280_spi_read_bytes(BMP280_REG_ID, &chip_id, 1) != STATUS_OK) {
return STATUS_ERROR;
}

if (chip_id != BMP280_CHIP_ID) {
return STATUS_ERROR;
}

// Soft reset
if (bmp280_spi_write_byte(BMP280_REG_RESET, 0xB6) != STATUS_OK) {
return STATUS_ERROR;
}
_delay_ms(10);

return bmp280_get_calib_data(dev);
}

status_t bmp280_configure(bmp280_dev_t *dev, bmp280_oversample_t osrs_t,
bmp280_oversample_t osrs_p, bmp280_power_mode_t mode,
bmp280_standby_t standby, bmp280_filter_t filter) {
if (!dev) return STATUS_ERROR;

uint8_t config_reg = (standby << 5) | (filter << 2);
if (bmp280_spi_write_byte(BMP280_REG_CONFIG, config_reg) != STATUS_OK) {
return STATUS_ERROR;
}

uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
if (bmp280_spi_write_byte(BMP280_REG_CTRL_MEAS, ctrl_meas_reg) != STATUS_OK) {
return STATUS_ERROR;
}

return STATUS_OK;
}

status_t bmp280_read_fixed(bmp280_dev_t *dev, int32_t *temperature, uint32_t *pressure) {
if (!dev) return STATUS_ERROR;

uint8_t data[6];

if (bmp280_spi_read_bytes(BMP280_REG_PRESS_MSB, data, 6) != STATUS_OK) {
return STATUS_ERROR;
}

int32_t adc_p = (int32_t)(((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4));
int32_t adc_t = (int32_t)(((uint32_t)data[3] << 12) | ((uint32_t)data[4] << 4) | ((uint32_t)data[5] >> 4));

// --- Temperature Compensation ---
int32_t var1, var2;
var1 = ((((adc_t >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) * ((int32_t)dev->calib.dig_T2)) >> 11;
var2 = (((((adc_t >> 4) - ((int32_t)dev->calib.dig_T1)) * ((adc_t >> 4) - ((int32_t)dev->calib.dig_T1))) >> 12) * ((int32_t)dev->calib.dig_T3)) >> 14;

dev->t_fine = var1 + var2;
if (temperature) {
*temperature = (dev->t_fine * 5 + 128) >> 8;
}

// --- Pressure Compensation ---
int64_t p_var1, p_var2, p;
p_var1 = ((int64_t)dev->t_fine) - 128000;
p_var2 = p_var1 * p_var1 * (int64_t)dev->calib.dig_P6;
p_var2 = p_var2 + ((p_var1 * (int64_t)dev->calib.dig_P5) << 17);
p_var2 = p_var2 + (((int64_t)dev->calib.dig_P4) << 35);
p_var1 = ((p_var1 * p_var1 * (int64_t)dev->calib.dig_P3) >> 8) + ((p_var1 * (int64_t)dev->calib.dig_P2) << 12);
p_var1 = (((((int64_t)1) << 47) + p_var1)) * ((int64_t)dev->calib.dig_P1) >> 33;

if (p_var1 == 0) {
return STATUS_ERROR;
}

p = 1048576 - adc_p;
p = (((p << 31) - p_var2) * 3125) / p_var1;
p_var1 = (((int64_t)dev->calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
p_var2 = (((int64_t)dev->calib.dig_P8) * p) >> 19;
p = ((p + p_var1 + p_var2) >> 8) + (((int64_t)dev->calib.dig_P7) << 4);

if (pressure) {
*pressure = (uint32_t)p;
}

return STATUS_OK;
}

status_t bmp280_read_float(bmp280_dev_t *dev, float *temperature, float *pressure) {
int32_t t_fixed;
uint32_t p_fixed;

if (bmp280_read_fixed(dev, &t_fixed, &p_fixed) != STATUS_OK) {
return STATUS_ERROR;
}

if (temperature) {
*temperature = (float)t_fixed / 100.0f;
}

if (pressure) {
*pressure = (float)p_fixed / 256.0f;
}

return STATUS_OK;
}