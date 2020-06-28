SRC = src/Timer/dcf.c src/Timer/display.c src/Timer/lessons.c src/Timer/menu.c src/Timer/main.c
CC = avr-gcc
CFLAGS = -funsigned-char -funsigned-bitfields -DF_CPU=8000000UL -Os -fno-move-loop-invariants -fno-tree-loop-optimize -fno-inline-small-functions -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -Wall -mmcu=attiny24 -std=gnu99 -flto

all: timer.elf
	avr-size -C --mcu=attiny24 timer.elf

clean:
	-rm timer.elf

timer.elf: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^
