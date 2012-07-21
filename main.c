#include <avr/wdt.h>
#include <util/delay.h>
#include <wiring.h>
#include "usbdrv.h"
#include <usbconfig.h>

void setup();
void loop();

int main() {
  setup();
  for(;;)
    loop();
}

void setup() {
  int i;
  /*
  usbInit();
  usbDeviceDisconnect();  // enforce re-enumeration, do this while interrupts are disabled!
  i = 500;
  while(--i){             // fake USB disconnect for > 250 ms
    wdt_reset();
    _delay_ms(1);
  }
  usbDeviceConnect();
*/
    usbSetup();
  pinMode(4, OUTPUT);
}

int state = LOW;

void loop() {
  wdt_reset();
  usbPoll();
  if(usbInterruptIsReady())
    usbSendKeyStroke(20);
  _delay_ms(20);
  //  digitalWrite(4, LOW);
  //  digitalWrite(4, state);
  //  state = (state == LOW) ? HIGH : LOW;
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
