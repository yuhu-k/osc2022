tool_chain = aarch64-linux-gnu-
SIMU_ENV = qemu-system-aarch64

SRC_DIR = src
DUM_DIR = out

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(DUM_DIR)/%.o)
FILESENDER = sendfile

.PHONY: all clean run debug

all: kernel8.img $(FILESENDER)

$(FILESENDER): $(FILESENDER).cpp
	g++ $@.cpp -o $@

kernel8.img: $(DUM_DIR) $(OBJS) $(DUM_DIR)/start.o $(SRC_DIR)/linker.ld
	$(tool_chain)ld -T $(SRC_DIR)/linker.ld -o kernel8.elf $(OBJS) $(DUM_DIR)/start.o
	$(tool_chain)objcopy -O binary kernel8.elf kernel8.img

$(DUM_DIR)/start.o: $(SRC_DIR)/start.S
	$(tool_chain)gcc -c $(SRC_DIR)/start.S -o $(DUM_DIR)/start.o 

$(DUM_DIR)/%.o: $(SRC_DIR)/%.c
	$(tool_chain)gcc -Wall -I include -c $< -o $@ -fno-stack-protector

$(DUM_DIR): 
	mkdir -p $(DUM_DIR)

clean:
	rm -rf kernel8.*
	rm -rf out
	rm -rf $(FILESENDER)

run: all
	$(SIMU_ENV) -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio

debug: 
	$(SIMU_ENV) -M raspi3 -kernel kernel8.img -display none -serial null -serial pty

send: $(FILESENDER)
	./$< $(args)

