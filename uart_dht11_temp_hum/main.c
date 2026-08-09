/**
* Reads data from the DHT11 sensor and display via UART protocol
* ref: https://components101.com/sites/default/files/component_datasheet/DFR0067%20DHT11%20Datasheet.pdf
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdarg.h>
#include <avr/interrupt.h>

#define RX_BUFFER_SIZE 32
#include <util/delay.h>


void uart_putc(char c);
char uart_getc(void);
void uart_puts(const char* s);
void uart_print_unsigned(unsigned long val);
void uart_print_signed(long val);
void uart_printf(const char* fmt, ...);

uint8_t uart_available(void);
char uart_getc_buffered(void);

uint8_t dht_read(uint8_t* hum, uint8_t* temp);


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



int main(void) {

	//DDRD |= (1 << DDD2);
	//PORTD &= ~(1 << PORTD2);
	//while(1);


	// baud rate
	UBRR0H = 0;
	UBRR0L = 103;

	UCSR0B = (1 << TXEN0) | (1 << RXEN0);// | (1 << RXCIE0);
	

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	sei();
	_delay_ms(1000);

	int temperature = -5;
	unsigned long counter = 0;

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


uint8_t dht_read(uint8_t* hum, uint8_t* temp){
	// check datasheet
	// DHT11
	uint8_t data[5] = {0, 0,0 ,0, 0};
	
	// start signal low signal (retention >=18ms)
	DDRD |= (1 << DDD2);
	PORTD &= ~(1 << PORTD2);
	_delay_ms(18);
	

	/**
	if(PIND & (1 << PIND2)) 
		uart_puts("1. PD2 is HIGH\r\n");
	else
		uart_puts("1. PD2 IS LOW\r\n");
	*/
	// switch back to input mode (PD2)

	PORTD |= (1 << PORTD2);
	DDRD &= ~(1 << DDD2);
	_delay_us(30);

	/**
	if(PIND & (1 << PIND2)) 
		uart_puts("2. PD2 is HIGH\r\n");
	else
		uart_puts("2. PD2 IS LOW\r\n");
		*/
	// DHT response low 80ms and high 80ms
	uint16_t timeout = 10000;

	while((PIND & (1 << PIND2)) && --timeout);
	if(timeout == 0) return 1;

	timeout = 10000;
	while(!(PIND & (1 << PIND2)) && --timeout);
	if(timeout == 0) return 2;

	timeout = 10000;
	while((PIND & (1 << PIND2)) && --timeout);
	if(timeout == 0) return 3;
	

	// read 5 bytes
	for(uint8_t i = 0; i < 40; i ++){
		timeout = 10000;
		while(!(PIND & (1 <<PIND2)) && --timeout);
		if(timeout == 0) return 4;

		_delay_us(30);
		data[i/8] <<= 1;
		
		if(PIND & (1 << PIND2)) {
			data[i/8] |= 1;

			while((PIND & (1 << PIND2)) && --timeout);
		}


	}


		if(data[4] != (uint8_t)(data[0] + data[1] + data[2] + data[3])) {
				return 5;
		}


		*hum = data[0];
		*temp = data[2];
		return 0;

}
