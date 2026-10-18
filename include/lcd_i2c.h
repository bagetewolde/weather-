#ifndef LCD_H
#define LCD_H

#include "../include/types.h"
#include <stdint.h>
void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_write_char(char c);
void lcd_print(const char *str);
void lcd_backlight_on(void);
void lcd_backlight_off(void);
void lcd_display_on(void);
void lcd_display_off(void);
bool lcd_is_backlight_on(void);

#endif /* LCD_H */
