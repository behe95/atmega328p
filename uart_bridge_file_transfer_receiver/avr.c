#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define FOSC 16000000
#define BAUD 9600
#define UBRR (((FOSC/16)/BAUD) - 1)

volatile uint8_t c; 
volatile int tx_ready;

void UART_init();


int main(void) {
  cli();
  UART_init();

  sei();
  while(1);
}


void UART_init() {
  UCSR0A = 0;
  UCSR0B = 0;
  UCSR0C = 0;

  UBRR0H = (UBRR >> 8);
  UBRR0L = UBRR;

  // 8 bit data.
  UCSR0C |= ((1 << UCSZ01) | (1 << UCSZ00));

  // enable interrupts
  // receive interrupt
  UCSR0B |= (1 << RXCIE0);

  // // enable data register empty interrupt
  // UCSR0B |= (1 << UDRIE0);


  // enable receiver and transmitter
  UCSR0B |= ((1 << RXEN0) | (1 << TXEN0));

}


// USART Rx Complete
ISR(USART_RX_vect) {
  c = UDR0;
  tx_ready = 1;

  // enable data register empty interrupt
  UCSR0B |= (1 << UDRIE0);
}

// USART Data Register Empty
ISR(USART_UDRE_vect) {
  if(tx_ready) {
    UDR0 = c;
    tx_ready = 0;
  } else {
    
    // disable data register empty interrupt
    UCSR0B &= ~(1 << UDRIE0);
  }
}

// USART Tx Complete
ISR(USART_TX_vect) {

}
