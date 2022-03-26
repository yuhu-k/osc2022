tool_chain = aarch64-linux-gnu-
SIMU_ENV = qemu-system-aarch64


.PHONY: all clean run debug tty

all: kernel8.img

kernel8.img: start.o linker.ld
	$(tool_chain)ld -T linker.ld -o kernel8.elf start.o 
	$(tool_chain)objcopy -O binary kernel8.elf kernel8.img

start.o: start.S
	$(tool_chain)gcc -g -Wall -I include -c $< -o $@


clean:
	rm -rf kernel8.*
	rm -rf *.o
