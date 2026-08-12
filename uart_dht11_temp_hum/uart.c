#include "uart.h"
#include <avr/io.h>
#include <stdarg.h>

#include <avr/interrupt.h>

volatile char rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_head = 0;
volatile uint8_t rx_tail = 0;


ISR(USART_RX_vect) {
	char c = UDR0;
	uint8_t next_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
	
	if(next_tail != rx_head) {
		rx_buffer[rx_tail] = c;
		rx_tail = next_tail;
	}
}


void uart_putc(char c) {
	
	while(!(UCSR0A & (1 << UDRE0)));

	UDR0 = c;
}


char uart_getc(void){
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}


void uart_puts(const char* s){
	while(*s) {
		uart_putc(*s);
		s++;
	}
}

void uart_print_unsigned(unsigned long val){
	char buf[10];
	uint8_t i = 0;
	if(val == 0) {
		uart_putc('0');
		return;
	}

	while(val > 0) {
		buf[i] = '0' + (val%10);
		i++;
		val = val / 10;
	}


	while(i > 0) {
		i--;
		uart_putc(buf[i]);
	}

}



void uart_print_signed(long val){
	if(val < 0) {
		uart_putc('-');
		val = -val;
	}

	uart_print_unsigned((unsigned long) val);
}


void uart_printf(const char* fmt, ...){
	// check man 
	// va_list va_start va_arg va_end

	va_list args;
	va_start(args, fmt);
	
	while(*fmt){
		if(*fmt == '%'){
			fmt++;
			switch(*fmt) {
				case 'd':
					uart_print_signed(va_arg(args, int));
					break;
				case 'u':
					uart_print_unsigned(va_arg(args, unsigned int));
					break;
				case 'l':
					fmt++;
					if(*fmt == 'd') {
						uart_print_signed(va_arg(args, long));
					} else if(*fmt == 'u') {
						uart_print_unsigned(va_arg(args, unsigned long));
					}
					break;
				case 'c':
					uart_putc((char) va_arg(args, int));
					break;
				case 's':
					uart_puts(va_arg(args, char*));
					break;
				case '%':
					uart_putc('%');
					break;
			}
		} else {
			uart_putc(*fmt);
		}
		fmt++;
	}


	va_end(args);

}

uint8_t uart_available(void) {
	return rx_head != rx_tail;
}

char uart_getc_buffered(void){
	while(rx_head == rx_tail);
	char c = rx_buffer[rx_head];
	rx_head = (rx_head + 1) % RX_BUFFER_SIZE;
	return c;
}

uint8_t uart_readline(char* buf, uint8_t maxlen) {
	uint8_t idx = 0;

	while(1) {
		if(uart_available()) {
			char c = uart_getc_buffered();
			if(c == '\n' || c == '\r') {
				buf[idx] = 0;
				uart_puts("\r\n");
				return idx;
			} else if(c == 8 || c == 127) {
				if(idx > 0) {
					idx--;
					uart_puts("\b \b");
				}
			} else if(idx < (uint8_t)(maxlen-1)) {
				buf[idx] = c;
				idx++;
				uart_putc(c);
			} 		
		}
	}

}
