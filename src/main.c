#include "../include/lcd_i2c.h"
#include "../include/bmp280.h"
#include <stdio.h>
#include <util/delay.h>

#define BUTTON_PIN PB0
#define BUTTON_PORT PORTB
#define BUTTON_DDR DDRB
#define BUTTON_IN PINB
int main(void) {
    //Initialise button

    BUTTON_DDR &= ~(1 << BUTTON_PIN);
    BUTTON_DDR |= (1 << BUTTON_PIN);
    uint8_t display_mode = 0;
    
    

    // 1. Initialize LCD display
    lcd_init();
    lcd_set_cursor(0, 0);
    lcd_print("Initializing...");
    

    // 2. Initialize BMP280 sensor
    bmp280_dev_t bmp280;
    if (bmp280_init(&bmp280) != STATUS_OK) {
        lcd_clear();
        lcd_print("BMP280 Error!");
        while (1);
    }

    // 3. Configure BMP280 (Temp x2, Pressure x16, Normal Mode, 1s Standby, Filter x4)
    bmp280_configure(&bmp280, 
                     BMP280_OSRS_X2, 
                     BMP280_OSRS_X16, 
                     BMP280_MODE_NORMAL, 
                     BMP280_STBY_1000, 
                     BMP280_FILTER_4);

    lcd_clear();
    float temp_c = 0.0f;
    float press_pa = 0.0f;
    char buffer[16];

    while (1) {
        if (!(PINB & (1 << PB0))) {
            _delay_ms(50);
            while(!(PINB & (1 << PB0))){
                _delay_ms(10);
            }
            display_mode = (display_mode + 1) % 3;
            lcd_clear();
        }
        
        
        
        
        switch (display_mode) {
            case 0:
                
                    if (bmp280_read_float(&bmp280, &temp_c, &press_pa) == STATUS_OK) {
            // Format Temperature

            int32_t temp_scaled = (int32_t)(temp_c * 100.0f);
            int temp_whole = temp_scaled / 100;
            int temp_frac = temp_scaled % 100;
            if (temp_frac < 0) temp_frac = -temp_frac;
            lcd_set_cursor(0, 0);
            snprintf(buffer, sizeof(buffer), "Temp: %d.%02d C", temp_whole, temp_frac);
            lcd_print(buffer);

            // Format Pressure (hPa = Pa / 100)
            int32_t press_scaled = (int32_t)(press_pa / 10.0f);
            int press_whole = press_scaled / 10;
            int press_frac = press_scaled % 10;
            if (press_frac < 0) press_frac = -press_frac;
            lcd_set_cursor(1, 0);
            snprintf(buffer, sizeof(buffer), "Press: %d.%02d hP", press_whole, press_frac);
            lcd_print(buffer);
        } else {
            lcd_set_cursor(0, 0);
            lcd_print("Read Error");
        }
                
            break;
            case 1:
                lcd_set_cursor(0, 0);
                lcd_print("GPS Daata");
                lcd_set_cursor(1, 0);
                lcd_print("3degreen");
                break;
            case 2:
                lcd_set_cursor(0, 0);
                lcd_print("Humidity");
                lcd_set_cursor(1, 0);
                lcd_print("tempo");
                break;
            }
            

        

        
    }
}
