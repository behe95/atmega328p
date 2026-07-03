#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define FOSC 16000000
#define BAUD 9600
#define UBRR (((FOSC/16)/BAUD) - 1)
#define BUFFER_SIZE 128

typedef struct _TCircBuff {
  volatile char* ptrRead;
  volatile char* ptrWrite;
  volatile char buffer[BUFFER_SIZE];
} _TCircBuff;

volatile uint8_t c; 
volatile int tx_ready;

volatile _TCircBuff buffer;

void UART_init();
void initBuf(_TCircBuff* buf);

char readBuf(_TCircBuff* buf);
void writeBuf(_TCircBuff* buf, char c);


int main(void) {
  initBuf(&buffer);

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
  writeBuf(&buffer, UDR0);

  // enable data register empty interrupt
  UCSR0B |= (1 << UDRIE0);
}

// USART Data Register Empty
ISR(USART_UDRE_vect) {
  char c = readBuf(&buffer);
  if(c == 0) {
      
    // disable data register empty interrupt
    UCSR0B &= ~(1 << UDRIE0);
    return;
  }
  
  UDR0 = c;


  
}

// USART Tx Complete
ISR(USART_TX_vect) {

}


void initBuf(_TCircBuff* buf) {
  buf->ptrRead = buf->buffer;
  buf->ptrWrite = buf->buffer;
}



char readBuf(_TCircBuff* buf) {
  if(buf->ptrRead == buf->ptrWrite) {
    return 0;
  }

  char c = *(buf->ptrRead);
  buf->ptrRead += 1;

  // ensure not exceeding size
  // if exceeding start from the 0
  if(buf->ptrRead >= (buf->buffer + BUFFER_SIZE)) {
    buf->ptrRead = buf->buffer;
  }

  return c;
}
void writeBuf(_TCircBuff* buf, char c) {

  char* next = buf->ptrWrite + 1;
  
  if(next >= (buf->buffer + BUFFER_SIZE)) {
    next = buf->buffer;
  }
  *(buf->ptrWrite) = c;
  buf->ptrWrite = next;

}
