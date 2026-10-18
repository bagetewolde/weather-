#include "../include/uart.h"
#include "../include/config.h"
#include <avr/io.h>

void uart_init(uint32_t baud_rate) {
    uint16_t ubrr = (uint16_t)((F_CPU / (16UL * baud_rate)) - 1);
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr);

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8-bit data, 1 stop bit */
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

char uart_receive(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

bool uart_available(void) {
    return (UCSR0A & (1 << RXC0)) != 0;
}

void uart_print(const char *str) {
    if (!str) return;
    while (*str) {
        uart_transmit(*str++);
    }
}

void uart_println(const char *str) {
    uart_print(str);
    uart_transmit('\r');
    uart_transmit('\n');
}

uint8_t uart_read_line(char *buffer, uint8_t max_len) {
    uint8_t count = 0;
    while (count < max_len - 1) {
        if (uart_available()) {
            char c = uart_receive();
            if (c == '\r') continue;
            if (c == '\n') break;
            buffer[count++] = c;
        }
    }
    buffer[count] = '\0';
    return count;
}
