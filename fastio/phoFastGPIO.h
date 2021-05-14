/**
 * phoFastGPIO
 * Copyright (C) 2016 Pho Hale
 *
 * Based on Marlin 3D Printer Firmware [https://github.com/MarlinFirmware/Marlin], Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * Fast I/O Routines for AVR
 * Use direct port manipulation to save scads of processor time.
 * Contributed by Pho Hale, Triffid_Hunter and modified by Kliment, thinkyhead, Bob-the-Kuhn, et.al.
 */


#ifndef _PHOFASTGPIO_H_
#define _PHOFASTGPIO_H_

#include <stdint.h>
#include <avr/io.h>

/**
 * Magic I/O routines
 *
 * Now you can simply SET_OUTPUT(PIN); WRITE(PIN, HIGH); WRITE(PIN, LOW);
 *
 * Why double up on these macros? see https://gcc.gnu.org/onlinedocs/cpp/Stringification.html
 */

#define _READ(IO)             TEST(DIO ## IO ## _RPORT, DIO ## IO ## _PIN)

#define _WRITE_NC(IO,V) do{ \
  if (V) SBI(DIO ## IO ## _WPORT, DIO ## IO ## _PIN); \
  else   CBI(DIO ## IO ## _WPORT, DIO ## IO ## _PIN); \
}while(0)

#define _WRITE_C(IO,V) do{ \
  uint8_t port_bits = DIO ## IO ## _WPORT;                  /* Get a mask from the current port bits */ \
  if (V) port_bits = ~port_bits;                            /* For setting bits, invert the mask */ \
  DIO ## IO ## _RPORT = port_bits & _BV(DIO ## IO ## _PIN); /* Atomically toggle the output port bits */ \
}while(0)

#define _WRITE(IO,V)          do{ if (&(DIO ## IO ## _RPORT) < (uint8_t*)0x100) _WRITE_NC(IO,V); else _WRITE_C(IO,V); }while(0)

#define _TOGGLE(IO)           (DIO ## IO ## _RPORT = _BV(DIO ## IO ## _PIN))

#define _SET_INPUT(IO)        CBI(DIO ## IO ## _DDR, DIO ## IO ## _PIN)
#define _SET_OUTPUT(IO)       SBI(DIO ## IO ## _DDR, DIO ## IO ## _PIN)

#define _IS_INPUT(IO)         !TEST(DIO ## IO ## _DDR, DIO ## IO ## _PIN)
#define _IS_OUTPUT(IO)        TEST(DIO ## IO ## _DDR, DIO ## IO ## _PIN)

// digitalRead/Write wrappers
#ifdef FASTIO_EXT_START
  void extDigitalWrite(const int8_t pin, const uint8_t state);
  uint8_t extDigitalRead(const int8_t pin);
#else
  #define extDigitalWrite(IO,V) digitalWrite(IO,V)
  #define extDigitalRead(IO)    digitalRead(IO)
#endif

#define READ(IO)              _READ(IO)
#define WRITE(IO,V)           _WRITE(IO,V)
#define TOGGLE(IO)            _TOGGLE(IO)

#define SET_INPUT(IO)         _SET_INPUT(IO)
#define SET_INPUT_PULLUP(IO)  do{ _SET_INPUT(IO); _WRITE(IO, HIGH); }while(0)
#define SET_INPUT_PULLDOWN    SET_INPUT
#define SET_OUTPUT(IO)        _SET_OUTPUT(IO)
#define SET_PWM               SET_OUTPUT

#define IS_INPUT(IO)          _IS_INPUT(IO)
#define IS_OUTPUT(IO)         _IS_OUTPUT(IO)

#define OUT_WRITE(IO,V)       do{ SET_OUTPUT(IO); WRITE(IO,V); }while(0)




// By looking at the pinout map above, you will find "PB7" next to pin 13.
// Arduino pin 13 is connected to ATMega2560 pin PB7 on the Arduino circuit board. Port B and bit 7.
// bit 7 in DDRB controls the direction (input/output)
// bit 7 in PORTB controls the output (high/low)
// You don't need to care about MCUCR, unless you want to disable pull-up on all pins.

// PA0


void phoFastDigitalWrite(uint8_t pin, uint8_t val)
{
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *out;

	if (port == NOT_A_PIN) return;
	out = portOutputRegister(port);

	uint8_t oldSREG = SREG;
	cli();

	if (val == LOW) {
		*out &= ~bit;
	} else {
		*out |= bit;
	}
	SREG = oldSREG;
}

// int phoFastDigitalRead(uint8_t pin)
// {
// 	uint8_t timer = digitalPinToTimer(pin);
// 	uint8_t bit = digitalPinToBitMask(pin);
// 	uint8_t port = digitalPinToPort(pin);

// 	if (port == NOT_A_PIN) return LOW;

// 	// If the pin that support PWM output, we need to turn it off
// 	// before getting a digital reading.
// 	if (timer != NOT_ON_TIMER) turnOffPWM(timer);

// 	if (*portInputRegister(port) & bit) return HIGH;
// 	return LOW;
// }



#endif // _PHOFASTGPIO_H_