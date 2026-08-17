/**
*
*  --------------------
*  |       DHT11       |
*  --------------------
*    S     3.3V   GND
*    |      |      |
*    PD5   3.3V   GND
*  --------------------
*  |       MCU         |
*  --------------------
*
*/



#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

uint8_t dht_read(uint8_t* hum, uint8_t* temp);

#endif
