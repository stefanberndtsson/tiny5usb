#include <util/delay.h>
#include <avr/wdt.h>
#include "usbconfig.h"
#include "usbkeyboard.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>

#include "usbdrv.h"

//typedef uint8_t byte;
#define BUFFER_SIZE 4 // Minimum of 2: 1 for modifiers + 1 for keystroke 

/* We use a simplifed keyboard report descriptor which does not support the
 * boot protocol. We don't allow setting status LEDs and but we do allow
 * simultaneous key presses. 
 * The report descriptor has been created with usb.org's "HID Descriptor Tool"
 * which can be downloaded from http://www.usb.org/developers/hidpage/.
 * Redundant entries (such as LOGICAL_MINIMUM and USAGE_PAGE) have been omitted
 * for the second INPUT item.
 */
PROGMEM char usbHidReportDescriptor[35] = { /* USB report descriptor */
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop) 
  0x09, 0x06,                    // USAGE (Keyboard) 
  0xa1, 0x01,                    // COLLECTION (Application) 
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard) 
  0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl) 
  0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI) 
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0) 
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1) 
  0x75, 0x01,                    //   REPORT_SIZE (1) 
  0x95, 0x08,                    //   REPORT_COUNT (8) 
  0x81, 0x02,                    //   INPUT (Data,Var,Abs) 
  0x95, BUFFER_SIZE-1,           //   REPORT_COUNT (simultaneous keystrokes) 
  0x75, 0x08,                    //   REPORT_SIZE (8) 
  //  0x25, 0x65,                    //   LOGICAL_MAXIMUM (101) 
  0x25, 0xA4,                    //   LOGICAL_MAXIMUM (164) 
  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated)) 
  //  0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application) 
  0x29, 0xA4,                    //   USAGE_MAXIMUM (Keyboard Application) 
  0x81, 0x00,                    //   INPUT (Data,Ary,Abs) 
  0xc0                           // END_COLLECTION 
};


static uchar    idleRate;           // in 4 ms units 
static uchar    reportBuffer[BUFFER_SIZE];    // buffer for HID reports [ 1 modifier byte + (len-1) key strokes]



void usbSetup () {
  uchar i;
  //  PORTB = 0; // TODO: Only for USB pins?
  //  DDRB |= ~USBMASK;
  wdt_enable(WDTO_1S);

  cli();

  usbInit();
  usbDeviceDisconnect();

    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }

  
  usbDeviceConnect();

      
  sei();

  // TODO: Remove the next two lines once we fix
  //       missing first keystroke bug properly.
  memset(reportBuffer, 0, sizeof(reportBuffer));      
  usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
}
    
void usbClearKeys(byte modifiers) {
  memset(reportBuffer, 0, sizeof(reportBuffer));
  reportBuffer[0] = modifiers;
  usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
}

void usbSendModifiers(byte modifiers) {
  reportBuffer[0] = modifiers;
  usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
}

void usbSendKeypress(byte keyStroke, byte modifiers) {
  int i;
  reportBuffer[0] = modifiers;
  // Search buffer if key is pressed already.
  for(i=1;i<BUFFER_SIZE;i++) {
    if(reportBuffer[i] == keyStroke) {
      return;
    }
  }
  // Key not in buffer. Find first zero slot and use that.
  for(i=1;i<BUFFER_SIZE;i++) {
    if(reportBuffer[i] == 0) {
      reportBuffer[i] = keyStroke;
      break;
    }
  }
  usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
}

void usbSendKeyrelease(byte keyStroke, byte modifiers) {
  int i;
  reportBuffer[0] = modifiers;
  // Search buffer if key is actually pressed.
  for(i=1;i<BUFFER_SIZE;i++) {
    if(reportBuffer[i] == keyStroke) {
      reportBuffer[i] = 0;
      usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
      return;
    }
  }
}

void usbSendKeyStroke(byte keyStroke, byte modifiers) {
  while (!usbInterruptIsReady()) {
    // Note: We wait until we can send keystroke
    //       so we know the previous keystroke was
    //       sent.
  }
      
  memset(reportBuffer, 0, sizeof(reportBuffer));

  reportBuffer[0] = modifiers;
  reportBuffer[1] = keyStroke;
        
  usbSetInterrupt(reportBuffer, sizeof(reportBuffer));

  while (!usbInterruptIsReady()) {
    // Note: We wait until we can send keystroke
    //       so we know the previous keystroke was
    //       sent.
  }
      
  // This stops endlessly repeating keystrokes:
  memset(reportBuffer, 0, sizeof(reportBuffer));      
  usbSetInterrupt(reportBuffer, sizeof(reportBuffer));

}

#if USB_CFG_HAVE_INTRIN_ENDPOINT3
void usbSendKeyStroke3(byte keyStroke, byte modifiers) {
      
  while (!usbInterruptIsReady3()) {
    // Note: We wait until we can send keystroke
    //       so we know the previous keystroke was
    //       sent.
  }
      
  memset(reportBuffer, 0, sizeof(reportBuffer));

  reportBuffer[0] = modifiers;
  reportBuffer[1] = keyStroke;
        
  usbSetInterrupt3(reportBuffer, sizeof(reportBuffer));

  while (!usbInterruptIsReady3()) {
    // Note: We wait until we can send keystroke
    //       so we know the previous keystroke was
    //       sent.
  }
      
  // This stops endlessly repeating keystrokes:
  memset(reportBuffer, 0, sizeof(reportBuffer));      
  usbSetInterrupt3(reportBuffer, sizeof(reportBuffer));

}
#endif
    
uchar usbFunctionSetup(uchar data[8]) {
  usbRequest_t    *rq = (usbRequest_t *)((void *)data);

  usbMsgPtr = reportBuffer; //
  if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
    /* class request type */

    if(rq->bRequest == USBRQ_HID_GET_REPORT){
      /* wValue: ReportType (highbyte), ReportID (lowbyte) */

      /* we only have one report type, so don't look at wValue */
      // TODO: Ensure it's okay not to return anything here?    
      return 0;
    } else if(rq->bRequest == USBRQ_HID_SET_REPORT) {
      return (rq->wLength.word == 1) ? USB_NO_MSG : 0;
    }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
      //            usbMsgPtr = &idleRate;
      //            return 1;
      return 0;
    }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
      idleRate = rq->wValue.bytes[1];
    }
  }else{
    /* no vendor specific requests implemented */
  }
  return 0;
}
