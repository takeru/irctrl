PRG            = irtest
OBJ            = main.o ir_ctrl.o uart.o xitoa.o
MCU_TARGET     = atmega48
OPTIMIZE       = -Os -mcall-prologues
DEFS           =
LIBS           =
DEBUG          = dwarf-2

CC             = avr-gcc
ASFLAGS        = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
ALL_ASFLAGS    = -mmcu=$(MCU_TARGET) -I. -x assembler-with-cpp $(ASFLAGS)
CFLAGS         = -g$(DEBUG) -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
LDFLAGS        = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump
SIZE           = avr-size



all: $(PRG).elf lst text eeprom size

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.bak *.a
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
	rm -rf $(PRG).hex
	rm -rf $(PRG).eep

size: $(PRG).elf
	$(SIZE) -C --mcu=$(MCU_TARGET) $(PRG).elf

lst:  $(PRG).lst
%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.o : %.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

text: hex
hex:  $(PRG).hex

eeprom: eep
eep:  $(PRG).eep

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eep: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

