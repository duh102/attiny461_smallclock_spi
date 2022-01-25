FLAGS = -mmcu=attiny461 -DF_CPU=8000000UL -O2 -std=c99 -Werror

test.hex: test.elf
	avr-objcopy -O ihex $< $@
	avr-size -C --mcu=attiny461 $<

clean:
	rm test.elf test.hex

test.elf: test.c
	avr-gcc $(FLAGS) $^ -o $@ 

test.c: segments.h colormap.h
