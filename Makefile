BUILD_DIR = ./build
OBJECT_DIR = ./obj
LINKER_DIR = ./linker
KERNEL_DIR = ./kernel
RISCV32_DIR = ./riscv32
LIBS_DIR = ./libs

KERNEL_DIR_ASSEMS = $(shell find $(KERNEL_DIR) -name "*.S")
RISCV32_DIR_ASSEMS = $(shell find $(RISCV32_DIR) -name "*.S")
KERNEL_DIR_SRCS = $(shell find $(KERNEL_DIR) -name "*.c")
RISCV32_DIR_SRCS = $(shell find $(RISCV32_DIR) -name "*.c")
RIBS_DIR_SRCS = $(shell find $(LIBS_DIR) -name "*.c")

# KERNEL_DIR_ASSEMS_OBJS = $(addprefix $(OBJECT_DIR)/, $(notdir $(KERNEL_DIR_ASSEMS:.S=.o)))
# RISCV32_DIR_ASSEMS_OBJS = $(addprefix $(OBJECT_DIR)/, $(notdir $(RISCV32_DIR_ASSEMS:.S=.o)))
# KERNEL_DIR_SRCS_OBJS = $(addprefix $(OBJECT_DIR)/, $(notdir $(KERNEL_DIR_SRCS:.c=.o)))
# RISCV32_DIR_SRCS_OBJS = $(addprefix $(OBJECT_DIR)/, $(notdir $(RISCV32_DIR_SRCS:.c=.o)))

KERNEL_DIR_ASSEMS_OBJS = $(addprefix $(OBJECT_DIR)/, $(KERNEL_DIR_ASSEMS:.S=.o))
RISCV32_DIR_ASSEMS_OBJS = $(addprefix $(OBJECT_DIR)/, $(RISCV32_DIR_ASSEMS:.S=.o))
KERNEL_DIR_SRCS_OBJS = $(addprefix $(OBJECT_DIR)/, $(KERNEL_DIR_SRCS:.c=.o))
RISCV32_DIR_SRCS_OBJS = $(addprefix $(OBJECT_DIR)/, $(RISCV32_DIR_SRCS:.c=.o))
RIBS_DIR_SRCS_OBJS = $(addprefix $(OBJECT_DIR)/, $(RIBS_DIR_SRCS:.c=.o))

TARGET = $(BUILD_DIR)/kernel.elf

CC=clang
CFLAGS=-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib -mno-relax -I ./

$(TARGET): $(KERNEL_DIR_ASSEMS_OBJS) $(RISCV32_DIR_ASSEMS_OBJS) $(KERNEL_DIR_SRCS_OBJS) $(RISCV32_DIR_SRCS_OBJS) $(RIBS_DIR_SRCS_OBJS)
	$(CC) $(CFLAGS) -Wl,-T$(LINKER_DIR)/kernel.ld -Wl,-Map=$(LINKER_DIR)/kernel.map -o $(TARGET) $(KERNEL_DIR_ASSEMS_OBJS) $(RISCV32_DIR_ASSEMS_OBJS) $(KERNEL_DIR_SRCS_OBJS) $(RISCV32_DIR_SRCS_OBJS) $(RIBS_DIR_SRCS_OBJS)

$(KERNEL_DIR_ASSEMS_OBJS): $(KERNEL_DIR_ASSEMS)
	$(CC) $(CFLAGS) -c $(<D)/$(patsubst %.o,%.S,$(@F)) -o $@

$(RISCV32_DIR_ASSEMS_OBJS): $(RISCV32_DIR_ASSEMS)
	$(CC) $(CFLAGS) -c $(<D)/$(patsubst %.o,%.S,$(@F)) -o $@

$(KERNEL_DIR_SRCS_OBJS): $(KERNEL_DIR_SRCS)
	$(CC) $(CFLAGS) -c $(<D)/$(patsubst %.o,%.c,$(@F)) -o $@

$(RISCV32_DIR_SRCS_OBJS): $(RISCV32_DIR_SRCS)
	$(CC) $(CFLAGS) -c $(<D)/$(patsubst %.o,%.c,$(@F)) -o $@

$(RIBS_DIR_SRCS_OBJS): $(RIBS_DIR_SRCS)
	$(CC) $(CFLAGS) -c $(<D)/$(patsubst %.o,%.c,$(@F)) -o $@


.PHONY: clean test
clean:
	-rm -rf $(BUILD_DIR)/* $(OBJECT_DIR)/*
test:
	echo $(BUILD_DIR)
	echo $(OBJECT_DIR)
	echo $(LINKER_DIR)
	echo $(KERNEL_DIR)
	echo $(RISCV32_DIR)
	echo $(SRC_DIRS)
	echo $(SRCS)
	echo $(ASSEMS)
	echo $(OBJS1)
	echo $(OBJS2)