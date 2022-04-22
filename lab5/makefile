tool_chain = aarch64-linux-gnu-
SIMU_ENV = qemu-system-aarch64
CC = gcc-10
CPP = g++-11

SRC_DIR = src
DUM_DIR = out

SRCS = $(wildcard $(SRC_DIR)/*.c)
SRCSS = $(wildcard $(SRC_DIR)/*.S)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(DUM_DIR)/%.o)
OBJSS = $(SRCSS:$(SRC_DIR)/%.S=$(DUM_DIR)/%.o)

.PHONY: all clean run debug tty

all: kernel8.img sender

kernel8.img: $(DUM_DIR) $(OBJS) $(OBJSS) $(SRC_DIR)/linker.ld
	$(tool_chain)ld -T $(SRC_DIR)/linker.ld -o kernel8.elf $(OBJS) $(OBJSS)
	$(tool_chain)objcopy -O binary kernel8.elf kernel8.img

$(DUM_DIR)/%.o: $(SRC_DIR)/%.S
	$(tool_chain)$(CC) -g -Wall -I include -c $< -o $@

$(DUM_DIR)/%.o: $(SRC_DIR)/%.c
	$(tool_chain)$(CC) -g -Wall -I include -c $< -o $@ -fno-stack-protector

$(DUM_DIR): 
	mkdir -p $(DUM_DIR)

sender: sendfile.cpp
	$(CPP) sendfile.cpp -o sender

clean:
	rm -rf kernel8.*
	rm -rf out
	rm sender

run: all
	$(SIMU_ENV) -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio -d int
tty:
	$(SIMU_ENV) -M raspi3 -kernel kernel8.img -display none -serial null -serial pty
debug: 
	$(SIMU_ENV) -M raspi3 -kernel kernel8.img -display none -S -s -initrd initramfs.cpio -d int
dev:
	$(SIMU_ENV) -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb 


