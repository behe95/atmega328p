#include "dht11.h"
#include <avr/io.h>

#include <util/delay.h>

#include <avr/interrupt.h>

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

