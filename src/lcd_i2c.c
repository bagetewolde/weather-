#include "../include/lcd_i2c.h"
#include "../include/i2c.h"
#include "../include/config.h"
#include "../include/system.h"
#include <util/delay.h>

#define LCD_BACKLIGHT_BIT   0x08
#define LCD_ENABLE_BIT      0x04
#define LCD_RS_BIT          0x01

static uint8_t backlight_state = LCD_BACKLIGHT_BIT;

static void lcd_send_nibble(uint8_t nibble, uint8_t mode) {
    uint8_t data = (nibble & 0xF0) | mode | backlight_state;

    twi_start();
    twi_write((LCD_I2C_ADDR << 1) | TWI_WRITE);
    twi_write(data | LCD_ENABLE_BIT);
    _delay_us(1);
    twi_write(data & ~LCD_ENABLE_BIT);
    _delay_us(50);
    twi_stop();
}

static void lcd_send_byte(uint8_t val, uint8_t mode) {
    lcd_send_nibble(val & 0xF0, mode);
    lcd_send_nibble((val << 4) & 0xF0, mode);
}

void lcd_init(void) {
    twi_init();
    _delay_ms(50);

    /* 4-bit initialization sequence */
    lcd_send_nibble(0x30, 0);
    _delay_ms(5);
    lcd_send_nibble(0x30, 0);
    _delay_us(150);
    lcd_send_nibble(0x30, 0);
    _delay_us(150);
    lcd_send_nibble(0x20, 0); /* Set 4-bit mode */
    _delay_us(150);

    lcd_send_byte(0x28, 0); /* 2-line, 5x8 dots */
    lcd_send_byte(0x0C, 0); /* Display ON, Cursor OFF */
    lcd_send_byte(0x06, 0); /* Entry mode: Increment cursor */
    lcd_clear();
}

void lcd_clear(void) {
    lcd_send_byte(0x01, 0);
    _delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > 3) row = 0;
    lcd_send_byte(0x80 | (col + row_offsets[row]), 0);
}

void lcd_write_char(char c) {
    lcd_send_byte((uint8_t)c, LCD_RS_BIT);
}

void lcd_print(const char *str) {
    if (!str) return;
    while (*str) {
        lcd_write_char(*str++);
    }
}

void lcd_backlight_on(void) {
    backlight_state = LCD_BACKLIGHT_BIT;
    twi_start();
    twi_write((LCD_I2C_ADDR << 1) | TWI_WRITE);
    twi_write(backlight_state);
    twi_stop();
}

void lcd_backlight_off(void) {
    backlight_state = 0x00;
    twi_start();
    twi_write((LCD_I2C_ADDR << 1) | TWI_WRITE);
    twi_write(backlight_state);
    twi_stop();
}

void lcd_display_on(void) {
    lcd_send_byte(0x0C, 0);
    lcd_backlight_on();
}

void lcd_display_off(void) {
    lcd_send_byte(0x08, 0); /* Display off */
    lcd_backlight_off();
}

bool lcd_is_backlight_on(void) {
    return (backlight_state == LCD_BACKLIGHT_BIT);
}
