#ifndef UART_H
#define UART_H

#include <stdint.h>


#define RX_BUFFER_SIZE 32


extern volatile char rx_buffer[RX_BUFFER_SIZE];
extern volatile uint8_t rx_head;
extern volatile uint8_t rx_tail;


void uart_putc(char c);
char uart_getc(void);
void uart_puts(const char* s);
void uart_print_unsigned(unsigned long val);
void uart_print_signed(long val);
void uart_printf(const char* fmt, ...);

uint8_t uart_available(void);
char uart_getc_buffered(void);

uint8_t uart_readline(char* buf, uint8_t maxlen);



#endif
