/**
*     Custom GPIO console
*     GPIO to 8 bit LED bit Mapping
*          |---------------------------------------------------------------|     
* LED      |       7bit--6bit--5bit--4bit--3bit--2bit--1bit--0bit          |
*          |         |     |     |     |     |     |     |     |           |
* GPIO     |         D2    D3    D4    D5    D6    D7    B0    B1          |
*          |---------------------------------------------------------------|
*
* PORTD ad PORTD bits are re-ordered to align with this LED layout
*
* Output:   8 bit value
* Use case: To inspect variables, internal register value, program state
*           during debugging.
*/

#include <stdint.h>
#include <avr/io.h>
#include "gpio_console.h"

void display(uint8_t b);
uint8_t reverse(int left, int len, uint8_t value);    // helper function


void display(uint8_t b) {
  DDRB |= 3;
  DDRD |= 252;

  PORTB = reverse(0, 2, b);
  PORTD = reverse(1, 6, b);
}


uint8_t reverse(int left, int len, uint8_t value) {

  uint8_t _val = value;

  // abcd efgh
  // efgh abcd
  // hgfe abcd
  // abcd hgfe

  // reverse right bits
  if(left <= 0) {
    // partition and swap
    _val = (uint8_t) (((_val >> len) & 255) | ((_val << (8-len)) & 255));
  }

  // left = 1
  // len = 3
  // abc defgh   // value

  // cba defgh    // output

  // if left = 1
  // abcdefgh
  // 11111111
  // 00011111
  //------------
  // 000defgh
  uint8_t unchanged = _val & (255 >> len);

  // abcdefgh
  // 11111111
  // 11100000
  //------------
  // abc00000
  uint8_t change = _val & (255 << (8-len));



  uint8_t temp = 0;

  int counter = 1;
  while(counter <= len) {

    uint8_t extracted = ((change >> (8-counter)) & 1);


    temp |= (extracted << (8-(len - counter+1)));


    counter++;
  }

  uint8_t fin = unchanged | temp;
  // swap
  if(left <= 0) {

    fin = (uint8_t)(((fin >> (8-len)) & 255) | ((fin << len) & 255));
  }
  return fin;
}
