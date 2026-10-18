#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include "../include/i2c.h"
#include "../include/system.h"

/* Chip ID */
#define BMP280_CHIP_ID 0x58

/* Register Addresses (MSB bit 7 must be 0 for write, 1 for read in SPI) */
#define BMP280_REG_ID 0xD0
#define BMP280_REG_RESET 0xE0
#define BMP280_REG_STATUS 0xF3
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG 0xF5
#define BMP280_REG_PRESS_MSB 0xF7
#define BMP280_REG_CALIB_START 0x88

/* Power Modes */
typedef enum {
BMP280_MODE_SLEEP = 0x00,
BMP280_MODE_FORCED = 0x01,
BMP280_MODE_NORMAL = 0x03
} bmp280_power_mode_t;

/* Oversampling Settings */
typedef enum {
BMP280_OSRS_SKIPPED = 0x00,
BMP280_OSRS_X1 = 0x01,
BMP280_OSRS_X2 = 0x02,
BMP280_OSRS_X4 = 0x03,
BMP280_OSRS_X8 = 0x04,
BMP280_OSRS_X16 = 0x05
} bmp280_oversample_t;

/* Standby Time Settings */
typedef enum {
BMP280_STBY_0_5 = 0x00,
BMP280_STBY_62_5 = 0x01,
BMP280_STBY_125 = 0x02,
BMP280_STBY_250 = 0x03,
BMP280_STBY_500 = 0x04,
BMP280_STBY_1000 = 0x05,
BMP280_STBY_2000 = 0x06,
BMP280_STBY_4000 = 0x07
} bmp280_standby_t;

/* Filter Coefficient Settings */
typedef enum {
BMP280_FILTER_OFF = 0x00,
BMP280_FILTER_2 = 0x01,
BMP280_FILTER_4 = 0x02,
BMP280_FILTER_8 = 0x03,
BMP280_FILTER_16 = 0x04
} bmp280_filter_t;

/* Calibration Data Structure */
typedef struct {
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;
} bmp280_calib_data_t;

/* Device Structure */
typedef struct {
bmp280_calib_data_t calib;
int32_t t_fine;
} bmp280_dev_t;

/* Public API Functions (Original Names Preserved) */
status_t bmp280_init(bmp280_dev_t *dev);
status_t bmp280_configure(bmp280_dev_t *dev, bmp280_oversample_t osrs_t,
bmp280_oversample_t osrs_p, bmp280_power_mode_t mode,
bmp280_standby_t standby, bmp280_filter_t filter);
status_t bmp280_read_fixed(bmp280_dev_t *dev, int32_t *temperature, uint32_t *pressure);
status_t bmp280_read_float(bmp280_dev_t *dev, float *temperature, float *pressure);
#endif /* BMP280_H */










