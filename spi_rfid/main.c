/**
* RFID RC522
*	Syed M.
*	Datasheet: https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf
*		Sections:
*			10.2 SPI Compatible Interface
*			9.1	MFRC522 Registers Overview
			12. FIFO buffer
*/

#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define F_CPU 16000000L
#define _RFID_ADDR_WRITE 0x00
#define _RFID_ADDR_READ 0x80	 // 10000000
#define _RFID_ADDR_MASK 0x7E		// 01111110

#define _RFID_VersionReg 0x37
#define _SPI_DUMMY_DATA 0x00

#define _RFID_CommandReg 0x01
#define _RFID_FIFODataReg 0x09	//FIFO buffer
#define _RFID_FIFOLevelReg 0x0A	// FIFO length tracer

#define _RFID_ErrorReg 0x06
#define _RFID_BufferOvfl 4

#define _RFID_WaterLevelReg 0x0B
#define _RFID_WaterLevel_MASK 0x3F//00111111

#define _RFID_Status1Reg 0x07
#define _RFID_HiAlert 0x01
#define _RFID_LoAlert 0x00


void setup_uart();
void setup_spi();
uint8_t spi_transfer(uint8_t data);
uint8_t rc522_read_reg(uint8_t reg);
void rc522_write_reg(uint8_t reg, uint8_t val);


int main(void) {
		
		setup_uart();
		setup_spi();

		_delay_us(40); // 16.11
		rc522_write_reg(_RFID_CommandReg, 0x0F);		// soft reset		
		rc522_write_reg(_RFID_FIFOLevelReg, 0x80);
	
		rc522_write_reg(_RFID_WaterLevelReg, 0x02 & _RFID_WaterLevel_MASK);	// set water level



		uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
	
		

		// write to FIFO
		for(uint8_t i = 0; i < 68; i++) {
			

			rc522_write_reg(_RFID_FIFODataReg, data[i%5]);
			uint8_t level = rc522_read_reg(_RFID_FIFOLevelReg);
			uart_printf("Wrote byte %u FIFOLevel = %u\r\n", i, level);
			if(rc522_read_reg(_RFID_ErrorReg) & (1 << _RFID_BufferOvfl)) {
				uart_printf("Error happened!!!!\r\n");
			}
			
			
			if(rc522_read_reg(_RFID_Status1Reg) & (1 << _RFID_LoAlert)) {
				uart_printf("FIFO current bytes is less than %u\r\n"
						, rc522_read_reg(_RFID_WaterLevelReg) & _RFID_WaterLevel_MASK);
			} else if (rc522_read_reg(_RFID_Status1Reg) & (1 << _RFID_HiAlert)) {
				uart_printf("FIFO remaining bytes is less than %u\r\n",
						rc522_read_reg(_RFID_WaterLevelReg) & _RFID_WaterLevel_MASK);
			}


		}
	
/*	
		//rc522_write_reg(_RFID_FIFOLevelReg, 0x80);
		// read from FIFO
		uint8_t level = rc522_read_reg(_RFID_FIFOLevelReg);
		for(uint8_t i = 0; i < level; i++) {
			uint8_t byte = rc522_read_reg(_RFID_FIFODataReg);
			uint8_t currlvl = rc522_read_reg(_RFID_FIFOLevelReg);
			uart_printf("Byte = %u Level = %u\r\n", byte, currlvl);
		}
*/
		while(1) {

		}
}

void setup_uart() {


		// uart config
		UBRR0H = 0;
		UBRR0L = 103;
		UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
		UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

}

void setup_spi() {


		// SPI config
		SPCR = (1 << SPE) | (1 << MSTR);
		
		// MOSI PINB3		SCK PINB5		SS PINB2
		DDRB |= (1 << DDB3) | (1 << DDB5) | (1 << DDB2);
		
		DDRD |= (1 << DDD3);
		PORTD |= (1 << PORTD3);
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


void rc522_write_reg(uint8_t reg, uint8_t val) {
	PORTB &= ~(1 << PORTB2);
	spi_transfer(_RFID_ADDR_WRITE | ((reg << 1) & _RFID_ADDR_MASK));
	spi_transfer(val);
	PORTB |= (1 << PORTB2);
}
