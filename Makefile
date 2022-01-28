FLAGS = -mmcu=attiny461 -DF_CPU=8000000UL -O2 -std=c99 -Werror

internal.hex: internal.elf
	avr-objcopy -O ihex $< $@
	avr-size -C --mcu=attiny461 $<

test.hex: test.elf
	avr-objcopy -O ihex $< $@
	avr-size -C --mcu=attiny461 $<

clean:
	rm -f test.elf test.hex internal.hex


internal.elf: internal.c
	avr-gcc $(FLAGS) $^ -o $@ 

test.elf: test.c tinyspi.c mcp79510.c
	avr-gcc $(FLAGS) $^ -o $@ 

test.c: segments.h colormap.h ws2812.h
	touch $@

internal.c: segments.h colormap.h ws2812.h
	touch $@

tinyspi.c: tinyspi.h
	touch $@

mcp79510.c: mcp79510.h
	touch $@

