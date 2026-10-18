#ifndef SOFT_UART_H
#define SOFT_UART_H

#include <stdint.h>
#include <stdbool.h>

void soft_uart_init(uint32_t baud_rate);
char soft_uart_receive(void);
bool soft_uart_available(void);
uint8_t soft_uart_read_line(char *buffer, uint8_t max_len);

#endif /* SOFT_UART_H */

