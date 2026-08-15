/**
* Reads data from the DHT11 sensor and display via UART protocol
* ref: https://components101.com/sites/default/files/component_datasheet/DFR0067%20DHT11%20Datasheet.pdf
*
*	ADD: CMD Line for User Input
*	Add: Stopwatch
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


// state
typedef enum {
	STATE_COUNTING,
	STATE_ALARMING,
	STATE_IDLE
} alarm_state_t;

volatile uint32_t ms_counter = 0;
volatile uint8_t new_second = 0;
volatile uint32_t second_counter = 0;
volatile uint16_t day_counter = 0;
//volatile alarm_state_t state = STATE_COUNTING;
//volatile uint16_t countdown_remaining = 10;
//volatile uint16_t blink_period_ms = 1000;
//volatile uint16_t blink_timer_ms = 0;


ISR(TIMER0_COMPA_vect) {
	ms_counter++;
	if(ms_counter >= 1000) {
		ms_counter = 0;
		new_second = 1;
		second_counter++;
		if(second_counter >= 86400) {
			second_counter = 0;
			day_counter++;
		}
	}

}



int main(void) {

	//DDRD |= (1 << DDD2);
	//PORTD &= ~(1 << PORTD2);
	//while(1);

	// uart config
	// baud rate
	UBRR0H = 0;
	UBRR0L = 103;

	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);


	// gpio config
	DDRB = (1 << DDB5);

	// timer0 config
	TCCR0A = (1 << WGM01);
	TCCR0B = (1 << CS01) | (1 << CS00);
	OCR0A = 249;
	TIMSK0 = (1 << OCIE0A);

	sei();
	//_delay_ms(1000);

	//int temperature = -5;
	//unsigned long counter = 0;
	

	char line[LINE_MAX];
	
	uart_puts("Type 'help' to show available commands\r\n");

	while(1) {
		//PORTB |= (1 << PORTB5);

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
		uart_puts("    3. sys_uptime\r\n");
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
	} else if(str_eq(line, "sys_uptime")) {
			uart_puts("\033[2J\033[H");
			uart_puts("System Up time (Press x to return)\r\n");
			while(1) {
				if(uart_available()) {
					char c = uart_getc_buffered();
					if(c == 'x') {
						uart_puts("\033[2J\033[H");
						break;
					}
				}

				if (new_second) {
					new_second = 0;

					uart_puts("\033[1K\r");
					uart_printf("%u days %u hours %u minutes %u seconds              ",
							(unsigned int) day_counter, 
							(unsigned int) ((second_counter / 3600) % 24),
							(unsigned int) ((second_counter / 60) % 60),
							(unsigned int) (second_counter % 60)
						);
				}
			}
	} else {
			uart_printf("Unknown command. Type 'help'\r\n");
	}
	

}
