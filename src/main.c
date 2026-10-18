#include "../include/lcd_i2c.h"
#include "../include/bmp280.h"
#include "../include/dht22.h"
#include <stdio.h>
#include <util/delay.h>
#include <math.h> // Required for altitude calculation (pow function)
#include "../include/scheduler.h"
#define BUTTON_PIN PB0
#define BUTTON_PORT PORTB
#define BUTTON_DDR DDRB
#define BUTTON_IN PINB

// Standard sea-level pressure in hPa for altitude calculation
#define SEALEVEL_PRESSURE_HPA 1013.25f

int main(void) {
    scheduler_init();
    // 1. Initialize button as INPUT with internal pull-up enabled
    BUTTON_DDR &= ~(1 << BUTTON_PIN);  // Set as input
    BUTTON_PORT |= (1 << BUTTON_PIN); // Enable pull-up resistor

    uint8_t display_mode = 0;
    
    // 2. Initialize LCD display
    lcd_init();
    lcd_set_cursor(0, 0);
    lcd_print("Initializing...");

    // 3. Initialize BMP280 sensor (SPI)
    bmp280_dev_t bmp280;
    if (bmp280_init(&bmp280) != STATUS_OK) {
        lcd_clear();
        lcd_print("BMP280 Error!");
        while (1);
    }

    // Configure BMP280 (Temp x2, Pressure x16, Normal Mode, 1s Standby, Filter x4)
    bmp280_configure(&bmp280, 
                     BMP280_OSRS_X2, 
                     BMP280_OSRS_X16, 
                     BMP280_MODE_NORMAL, 
                     BMP280_STBY_1000, 
                     BMP280_FILTER_4);

    // 4. Initialize DHT22 sensor (e.g., on Port D, Pin 4 / PD4)
    // Make sure to add a 4.7k pull-up resistor between the DHT22 data pin and 3.3V!
    dht_init();

    lcd_clear();
    float temp_bmp = 0.0f;
    float press_pa = 0.0f;
    uint16_t hum_raw = 0;
    uint16_t temp_raw = 0;
    uint8_t is_neg = 0;
    char buffer[20];

    // Non-blocking timer variables for fast, smooth updates
    uint32_t last_refresh_tick = 0;
    const uint32_t refresh_interval_ms = 300; // Refresh display every 300ms

    while (1) {
        // --- Instant Button Check (Runs every loop iteration) ---
        if (!(BUTTON_IN & (1 << BUTTON_PIN))) {
            _delay_ms(30); // Debounce
            if (!(BUTTON_IN & (1 << BUTTON_PIN))) {
                display_mode = (display_mode + 1) % 3; // Cycles 0, 1, 2
                lcd_clear();
                // Wait for button release so it doesn't double-click
                while (!(BUTTON_IN & (1 << BUTTON_PIN)));
            }
        }

        // --- Fast Non-Blocking Display Refresh ---
        // This keeps the loop spinning fast for the button, 
        // but throttles the I2C LCD updates so it updates smoothly without lagging.
        if (scheduler_now_ms() - last_refresh_tick >= refresh_interval_ms) {
            last_refresh_tick = scheduler_now_ms();

            // Fetch live sensor data
            bmp280_read_float(&bmp280, &temp_bmp, &press_pa);
            // Inside your main while(1) loop:
char gps_buffer[82];

// Check if software serial has a complete NMEA line from the GPS
if (soft_uart_available()) {
    if (soft_uart_read_line(gps_buffer, sizeof(gps_buffer)) > 0) {
        parse_nmea_sentence(gps_buffer, &my_gps);
    }
}
            

            switch (display_mode) {
                case 0: {
                    // --- Case 0: Pressure & Altitude ---
                    float press_hpa = press_pa / 100.0f;
                    
                    // Calculate Altitude using the barometric formula
                    // Altitude = 44330 * (1 - (P / P0)^(1/5.255))
                    float altitude_m = 44330.0f * (1.0f - pow(press_hpa / SEALEVEL_PRESSURE_HPA, 0.1903f));

                    // Print Pressure
                    int32_t press_scaled = (int32_t)(press_hpa * 10.0f);
                    lcd_set_cursor(0, 0);
                    snprintf(buffer, sizeof(buffer), "P: %ld.%2ld hP  ", press_scaled / 10, press_scaled % 10);
                    lcd_print(buffer);

                    // Print Altitude
                    int32_t alt_scaled = (int32_t)(altitude_m * 10.0f);
                    lcd_set_cursor(1, 0);
                    snprintf(buffer, sizeof(buffer), "Alt: %ld.%2ld m   ", alt_scaled / 10, alt_scaled % 10);
                    lcd_print(buffer);
                    break;
                }

                case 1:
                    // --- Case 1: GPS Data (Placeholder) ---
                    lcd_set_cursor(0, 0);
        if (my_gps.valid) {
            snprintf(buffer, sizeof(buffer), "Lat: %s     ", my_gps.latitude);
        } else {
            snprintf(buffer, sizeof(buffer), "GPS: Searching..");
        }
        lcd_print(buffer);

        lcd_set_cursor(1, 0);
        if (my_gps.valid) {
            snprintf(buffer, sizeof(buffer), "Lon: %s     ", my_gps.longitude);
        } else {
            snprintf(buffer, sizeof(buffer), "No Fix Yet    ");
        }
        lcd_print(buffer);
        break;

                case 2: {
                    if(dht_read(&temp_raw, &hum_raw, &is_neg)){
                        // Print Humidity
                            lcd_set_cursor(0, 0);
                            snprintf(buffer, sizeof(buffer), "Hum: %u.%u %%  ", hum_raw / 10, hum_raw % 10);
                            lcd_print(buffer);

                            // Print Temperature from DHT22
                            lcd_set_cursor(1, 0);
                            snprintf(buffer, sizeof(buffer), "T: %s%u.%u C  ", is_neg ? "-": "", temp_raw / 10, temp_raw % 10);
                            lcd_print(buffer);
                    }
                    
                    break;
                }
            }
        }
    }
}
