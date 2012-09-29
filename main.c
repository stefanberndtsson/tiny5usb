#include <avr/wdt.h>
#include <util/delay.h>
#include <wiring.h>
#include "usbkeyboard.h"
#include "usbdrv.h"
#include <usbconfig.h>
#include <softserial.h>
#include "keymap.h"

const int hexdata[16] = {KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7,
                         KEY_8, KEY_9, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F};
int keystate = KEYUP;
int LED_state = 0;
int modifier_value = 0x00;

void send_hex(int value) {
  usbSendKeyStroke(hexdata[(value>>4)&0xf], 0);
  usbSendKeyStroke(hexdata[value&0xf], 0);
}

void toggle_LED_state(int bit) {
  if(LED_state&(1<<bit)) {
    LED_state &= ~(1<<bit);
  } else {
    LED_state |= (1<<bit);
  }

  ss_write(0x0e);
  ss_write(LED_state);
}

void setup();
void loop();

int main() {
  setup();
  for(;;)
    loop();
}

void setup() {
  usbSetup();
  ss_setup(3, 4, true);
  ss_begin(1200);
}

void loop() {
  if(ss_available() <= 0) {
    wdt_reset();
    usbPoll();
  } else {
    wdt_reset();
    int value = ss_read(); 

    keystate = (value > 0x7f) ? KEYUP : KEYDOWN;
    value &= 0x7f;

    if(scancodes[value] == 71 && keystate == KEYUP) { // ScrLock
      toggle_LED_state(2);
    }

    if(scancodes[value] == 83 && keystate == KEYUP) { // NumLock
      toggle_LED_state(0);
    }

    if(scancodes[value] == 57 && keystate == KEYUP) { // CapsLock
      toggle_LED_state(3);
    }

    if(scancodes[value] > 0xdf) { // Modifier
      if(keystate == KEYDOWN) {
        modifier_value |= 1<<(scancodes[value]&0x7);
        usbSendModifiers(modifier_value);
      } else {
        modifier_value = modifier_value & ~(1<<(scancodes[value]&0x7));
        usbSendModifiers(modifier_value);
      }
    } else if(value == 0x7f) {
      usbClearKeys(modifier_value);
    } else {
      if(keystate == KEYDOWN) {
        usbSendKeypress(scancodes[value], modifier_value);
      } else {
        usbSendKeyrelease(scancodes[value], modifier_value);
      }
    }
  }
  _delay_ms(20);
}

void calibrateOscillator(void)
{
uchar       step = 128;
uchar       trialValue = 0, optimumValue;
int         x, optimumDev, targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

    /* do a binary search: */
    do{
        OSCCAL = trialValue + step;
        x = usbMeasureFrameLength();    /* proportional to current real frequency */
        if(x < targetValue)             /* frequency still too low */
            trialValue += step;
        step >>= 1;
    }while(step > 0);
    /* We have a precision of +/- 1 for optimum OSCCAL here */
    /* now do a neighborhood search for optimum value */
    optimumValue = trialValue;
    optimumDev = x; /* this is certainly far away from optimum */
    for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
        x = usbMeasureFrameLength() - targetValue;
        if(x < 0)
            x = -x;
        if(x < optimumDev){
            optimumDev = x;
            optimumValue = OSCCAL;
        }
    }
    OSCCAL = optimumValue;
}

void hadUsbReset(void) {
  calibrateOscillator();
}
