#include "../include/soft_uart.h"
#include <avr/io.h>
#include <util/delay.h>

// Define Software RX Pin (PD3 / Digital Pin 3 on ATmega328P/Nano)
#define SOFT_RX_PIN PIND3
#define SOFT_RX_PORT PORTD
#define SOFT_RX_DDR DDRD
#define SOFT_RX_PIN_REG PIND

void soft_uart_init(uint32_t baud_rate) {
    (void)baud_rate; // Fixed timing configuration for 9600 baud below
    SOFT_RX_DDR &= ~(1 << SOFT_RX_PIN); // Set as input
    SOFT_RX_PORT |= (1 << SOFT_RX_PIN); // Enable internal pull-up
}

bool soft_uart_available(void) {
    // Detects start bit (falling edge / LOW state on the RX pin)
    return (SOFT_RX_PIN_REG & (1 << SOFT_RX_PIN)) == 0;
}

char soft_uart_receive(void) {
    char data = 0;
    
    // Wait for start bit (LOW)
    while (SOFT_RX_PIN_REG & (1 << SOFT_RX_PIN));
    
    // Delay to center of start bit (approx 52us for 9600 baud)
    _delay_us(52); 

    // Sample 8 data bits (LSB first)
    for (uint8_t i = 0; i < 8; i++) {
        _delay_us(104); // Full bit duration for 9600 baud
        if (SOFT_RX_PIN_REG & (1 << SOFT_RX_PIN)) {
            data |= (1 << i);
        }
    }

    // Wait for stop bit
    _delay_us(104);
    return data;
}

uint8_t soft_uart_read_line(char *buffer, uint8_t max_len) {
    uint8_t count = 0;
    while (count < max_len - 1) {
        if (soft_uart_available()) {
            char c = soft_uart_receive();
            if (c == '\r') continue;
            if (c == '\n') break;
            buffer[count++] = c;
        }
    }
    buffer[count] = '\0';
    return count;
}

