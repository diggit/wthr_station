AVR_CC			= avr-gcc
AVR_LD			= $(AVR_CC)
AVR_OBJCOPY		= avr-objcopy
AVR_SIZE		= avr-size
MCU			= atmega8
CFLAGS			= -std=c99 -c -Wall -g -$(OPTIMIZE) -mmcu=$(MCU) -fdata-sections -ffunction-sections
OPTIMIZE		= O2
LDFLAGS			= -Wl,--gc-sections
TARGET			= main
UART_DEV		= /dev/ttyUSB0

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

all: $(TARGET).hex

$(TARGET):  $(OBJECTS)
	$(AVR_LD) -o $@.elf $^ $(LDFLAGS) -mmcu=$(MCU)

# You don't even need to be explicit here,
# compiling C files is handled automagically by Make.

%.o: %.c
	$(AVR_CC) $(CFLAGS) $^ -o $@

%.hex: $(TARGET) size
	$(AVR_OBJCOPY) -j .text -j .data -O ihex $<.elf $@

clean:
	rm -rf *.o $(TARGET).elf $(TARGET).hex

size: $(TARGET)
	$(AVR_SIZE) -C --mcu=$(MCU) $(TARGET).elf

install: clean load

load: $(TARGET).hex
	sudo avrdude -p m8 -c usbasp -U flash:w:$< 

loaduino: $(TARGET).hex
	echo "R" > $(UART_DEV)
	avrdude -V -p m8 -c arduino -P $(UART_DEV) -b 19200 -U flash:w:$<
