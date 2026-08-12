/**
* Reads data from the DHT11 sensor and display via UART protocol
* ref: https://components101.com/sites/default/files/component_datasheet/DFR0067%20DHT11%20Datasheet.pdf
*
*	ADD: CMD Line for User Input
*/


#include "uart.h"
#include "dht11.h"

#define F_CPU 16000000UL
#define LINE_MAX 32

#include <stdarg.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void handle_cmd(char* line);


uint8_t str_eq(const char* a, const char* b);





int main(void) {

	//DDRD |= (1 << DDD2);
	//PORTD &= ~(1 << PORTD2);
	//while(1);


	// baud rate
	UBRR0H = 0;
	UBRR0L = 103;

	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	sei();
	_delay_ms(1000);

	int temperature = -5;
	unsigned long counter = 0;
	

	char line[LINE_MAX];
	
	uart_puts("Type 'help' to show available commands\r\n");

	while(1) {
		
		uart_puts("> ");
		uart_readline(line, LINE_MAX);
		handle_cmd(line);
	}
	/*
	while(1) {
		//if(uart_available()) {
		//	char c = uart_getc_buffered();
		//	uart_putc(c);
		//}


		uint8_t hum, temp;
		uint8_t err = dht_read(&hum, &temp);

		if(err == 0) {
			uart_puts("Humidity: ");
			uart_print_unsigned(hum);
			uart_puts("% Temp: ");
			uart_print_unsigned(temp);
			uart_puts("C\r\n");
		} else {
			uart_puts("Read fail. Error Code: ");
			uart_print_unsigned(err);
			uart_puts("\r\n");
		}

		_delay_ms(2000);

	}*/

}




// character by character comparison
uint8_t str_eq(const char* a, const char* b) {
	while(*a && *b){
		if(*a != *b) {
			return 0;
		}
		a++;
		b++;
	}	

	return *a == 0 && *b ==0;
}




void handle_cmd(char* line) {
	if(!(*line)) {
		return;
	}

	if(str_eq(line, "help")) {
		uart_puts("        Commands        \r\n");
		uart_puts("    1. show_temp\r\n");
		uart_puts("    2. show_humidity\r\n");
	} else if(str_eq(line, "show_temp")) {
		uint8_t temp, hum;
		uint8_t err = dht_read(&hum, &temp);
		if(err == 0) {
			uart_printf("Temperature: %u C\r\n", (unsigned int)temp);
		} else {
			uart_printf("DHT11 error code: %u\r\n", (unsigned int) err);
		} 
	} else if(str_eq(line, "show_humidity")) {
		uint8_t temp, hum;
		uint8_t err = dht_read(&hum, &temp);
		if(err == 0) {
			uart_printf("Humidity: %u %%\r\n", (unsigned int)hum);
		} else {
			uart_printf("DHT11 error code: %u\r\n", (unsigned int) err);
		}
	} else {
			uart_printf("Unknown command. Type 'help'\r\n");
	}
	

}
