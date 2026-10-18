#ifndef UART_H
#define UART_H

#include "types.h"

void uart_init(uint32_t baud_rate);
void uart_transmit(char data);
char uart_receive(void);
bool uart_available(void);
void uart_print(const char *str);
void uart_println(const char *str);
uint8_t uart_read_line(char *buffer, uint8_t max_len);

#endif /* UART_H */
