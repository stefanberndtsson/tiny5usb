/*
SoftwareSerial.h (formerly NewSoftSerial.h) - 
Multi-instance software serial library for Arduino/Wiring
-- Interrupt-driven receive and other improvements by ladyada
   (http://ladyada.net)
-- Tuning, circular buffer, derivation from class Print/Stream,
   multi-instance support, porting to 8MHz processors,
   various optimizations, PROGMEM delay tables, inverse logic and 
   direct port writing by Mikal Hart (http://www.arduiniana.org)
-- Pin change interrupt macros by Paul Stoffregen (http://www.pjrc.com)
-- 20MHz processor support by Garrett Mace (http://www.macetech.com)
-- ATmega1280/2560 support by Brett Hagman (http://www.roguerobotics.com/)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at
http://arduiniana.org.
*/

#ifndef __SOFTSERIAL_H__
#define __SOFTSERIAL_H__

#include <wiring.h>
#include <pins_arduino.h>
#include <inttypes.h>

/******************************************************************************
* Definitions
******************************************************************************/

#define _SS_MAX_RX_BUFF 64 // RX buffer size
#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

  // per object data
  int write_error;
  uint8_t _receivePin;
  uint8_t _receiveBitMask;
  volatile uint8_t *_receivePortRegister;
  uint8_t _transmitBitMask;
  volatile uint8_t *_transmitPortRegister;

  uint16_t _rx_delay_centering;
  uint16_t _rx_delay_intrabit;
  uint16_t _rx_delay_stopbit;
  uint16_t _tx_delay;

  uint16_t _buffer_overflow;
  uint16_t _inverse_logic;

  // static data
  static char _receive_buffer[_SS_MAX_RX_BUFF]; 
  static volatile uint8_t _receive_buffer_tail;
  static volatile uint8_t _receive_buffer_head;
  static void *active_object;

  // private methods
  void ss_recv();
  uint8_t ss_rx_pin_read();
  void ss_tx_pin_write(uint8_t pin_state);
  void ss_setTX(uint8_t transmitPin);
  void ss_setRX(uint8_t receivePin);

  // private static method for timing
  static inline void ss_tunedDelay(uint16_t delay);
  void setWriteError(int err) { write_error = err; }

  // public methods
  void ss_setup(uint8_t receivePin, uint8_t transmitPin, int inverse_logic);
  void ss_begin(long speed);
  int ss_listen();
  void ss_end();
int ss_isListening() { return 1; }
  int ss_overflow() { int ret = _buffer_overflow; _buffer_overflow = false; return ret; }
  int ss_peek();

  size_t ss_write(uint8_t byte);
  int ss_read();
  int ss_available();
  void ss_flush();
  
//  using Print::write;

  // public only for easy access by interrupt handlers
  static inline void ss_handle_interrupt();



#endif
