TINYLIBDIR=/home/stefan/src/avr/tinyarduino
AVRDUDE=/usr/share/arduino/hardware/tools/avrdude
MODEL=attiny85
PORT=/dev/ttyUSB0
SPEED=19200
STK=stk500v1
F_CPU=16500000
DUDEBASEFLAGS=-C$(AVRDUDE).conf -v -p$(MODEL) -c$(STK) -P$(PORT) -b$(SPEED)
PROGNAME=tinyusb
FLASHFILE=$(PROGNAME).hex
DUDEFLASHFLAGS=-Uflash:w:$(FLASHFILE):i

SRCSC=main.c usbdrv.c usbkeyboard.c softserial.c
SRCSCPP=
SRCSASM=usbdrvasm.S

OBJS=$(SRCSC:.c=.o) $(SRCSASM:.S=.o) $(SRCSCPP:.cpp=.o)
LIBS=-ltinyarduino

CFLAGS=-Os -DF_CPU=$(F_CPU) -mmcu=$(MODEL) -I$(TINYLIBDIR) -I. -Wall -ffunction-sections -fdata-sections
CXXFLAGS=-Os -DF_CPU=$(F_CPU) -mmcu=$(MODEL) -I$(TINYLIBDIR) -I. -Wall -ffunction-sections -fdata-sections
ASFLAGS = -DF_CPU=$(F_CPU) -mmcu=$(MODEL) -I. -x assembler-with-cpp
LDFLAGS=-Wl,--gc-sections -mmcu=$(MODEL) -L$(TINYLIBDIR)
EEPFLAGS=-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0
FLASHFLAGS=-O ihex -R .eeprom

all: $(FLASHFILE)

.cpp.o:
	avr-g++ $(CXXFLAGS) -c $< -o $@

.c.o:
	avr-gcc $(CFLAGS) -c $< -o $@

.S.o:
	avr-gcc $(ASFLAGS) -c $< -o $@

$(FLASHFILE): $(PROGNAME).eep $(PROGNAME).elf
	avr-objcopy $(FLASHFLAGS) $(PROGNAME).elf $(FLASHFILE)

$(PROGNAME).eep: $(PROGNAME).elf
	avr-objcopy $(ELFFLAGS) $(PROGNAME).elf $(PROGNAME).eep

$(PROGNAME).elf: $(OBJS)
	avr-gcc $(LDFLAGS) -o $(PROGNAME).elf $(OBJS) $(LIBS)

flash:
	$(AVRDUDE) $(DUDEBASEFLAGS) $(DUDEFLASHFLAGS)

verify:
	$(AVRDUDE) $(DUDEBASEFLAGS)

clean:
	rm -f *~ *.o $(FLASHFILE) $(PROGNAME).eep $(PROGNAME).elf
