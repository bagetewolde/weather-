#ifndef CONFIG_H
#define CONFIG_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* Environmental sensors */
#define DHT22_PIN       PD4

/* BME280 I2C addresses */
#define BME280_ADDR_LOW     0x76
#define BME280_ADDR_HIGH    0x77
#define BME280_REG_CHIP_ID  0xD0
#define BME280_CHIP_ID      0x60
/* 16x2 LCD with PCF8574 I2C backpack */
#define LCD_I2C_ADDR       0x27
#define LCD_COLUMNS        16
#define LCD_ROWS           2

#define LCD_UPDATE_MS      500UL
/* Hardware USART: NEO-7M GPS */
#define GPS_BAUD        9600UL

/* Software UART: HC-05 Bluetooth */
#define BT_BAUD         9600UL
#define BT_RX_PIN       PB0
#define BT_TX_PIN       PB1

/* Status LED */
#define STATUS_LED_PIN  PD6

/* Scheduler */
#define SCHEDULER_TICK_HZ   1000UL
#define I2C_SCL_HZ          100000UL

#endif