/**
* RFID RC522
*	Syed M.
*	Datasheet: https://www.alldatasheet.com/view.jsp?Searchword=RC522
*		Sections:
*			10.2 SPI Compatible Interface
*			9.1	MFRC522 Registers Overview
			12. FIFO buffer
*/

#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000L
#define _RFID_ADD_WRITE 0x00
#define _RFID_ADDR_READ 0x80	 // 10000000
#define _RFID_ADDR_MASK 0x7E		// 01111110

#define _RFID_VersionReg 0x37
#define _SPI_DUMMY_DATA 0x00

uint8_t spi_transfer(uint8_t data);
uint8_t rc522_read_reg(uint8_t reg);
uint8_t rc522_write_reg(uint8_t reg, uint8_t val);


int main(void) {
		
		// uart config
		UBRR0H = 0;
		UBRR0L = 103;
		UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
		UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);


		// SPI config
		SPCR = (1 << SPE) | (1 << MSTR);
		
		// MOSI PINB3		SCK PINB5		SS PINB2
		DDRB |= (1 << DDB3) | (1 << DDB5) | (1 << DDB2);
		
		DDRD |= (1 << DDD3);
		PORTD |= (1 << PORTD3);

		int counter = 0;

		while(1) {
			//uart_puts("Hello");

		counter++;

		//uint8_t rfid_version = rc522_read_reg(_RFID_VersionReg);

		//uart_printf("Version: %u\r\n", rfid_version);
		
		//	uint8_t value = rc522_read_reg(0x01);
		//	uart_printf("CommandReg: %u\r\n", value);
		//	if(counter >= 10) break;


			


		}
}


uint8_t spi_transfer(uint8_t data) {
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}
uint8_t rc522_read_reg(uint8_t reg) {
	PORTB &= ~(1 << PORTB2);
	spi_transfer(_RFID_ADDR_READ | ((reg << 1) & _RFID_ADDR_MASK));
	uint8_t value = spi_transfer(_SPI_DUMMY_DATA);
	PORTB |= (1 << PORTB2);
	return value;
}


uint8_t rc522_write_reg(uint8_t reg, uint8_t val) {
	PORTB &= ~(1 << PORTB2);
	spi_transfer(_RFID_ADDR_WRITE | ((reg << 1) | _RFID_ADDR_MASK));
	spi_transfer(value);
	PORTB |= (1 << PORTB2);
}
