TOOL_CHAIN_PREFIX = aarch64-linux-gnu
CC = gcc

.PHONY: all env clean debug run

all: env kernel8.img

kernel8.img: kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

kernel8.elf: a.o
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf a.o

a.o:
	aarch64-linux-gnu-gcc -c a.S
	
env: 
	apt-get install $(CC)-$(TOOL_CHAIN_PREFIX)
	apt-get install qemu

clean:
	rm -rf kernel8.*
	rm -rf a.o

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s
