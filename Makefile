OBJ_DIR := build
ISO_DIR := iso
INCLUDE := -I include

SRCS := $(shell find . -name '*.c' -o -name '*.S' -o -name '*.asm' | sed 's|^./||')
OBJS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(filter %.c,$(SRCS))) \
        $(patsubst %.S, $(OBJ_DIR)/%.o, $(filter %.S,$(SRCS))) \
        $(patsubst %.asm, $(OBJ_DIR)/%.o, $(filter %.asm,$(SRCS)))
OBJS := $(filter-out $(OBJ_DIR)/boot/boot.o, $(OBJS))

CC := x86_64-elf-gcc-15.1.0
AS := x86_64-elf-as
NASM := nasm
LD := x86_64-elf-ld
CFLAGS := -ffreestanding  -Wall -Wextra -m64 $(INCLUDE) --std=c99 -Werror=implicit-function-declaration
LDFLAGS := -T linker.ld -nostdlib --allow-multiple-definition

all: $(ISO_DIR)/boot/kernel.elf grub

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "CC		$<"

$(OBJ_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@$(AS) --64 $< -o $@
	@echo "AS		$<"

$(OBJ_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	@$(NASM) -f elf64 $< -o $@
	@echo "NASM		$<"

$(ISO_DIR)/boot/kernel.elf: $(OBJ_DIR)/boot/boot.o $(filter-out $(OBJ_DIR)/boot/boot.o,$(OBJS))
	@mkdir -p $(ISO_DIR)/boot
	@$(LD) -n -T linker.ld -o $@ $^
	@echo "LD		$<"

grub:
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o kernel.iso $(ISO_DIR)

clean:
	@rm -rf $(OBJ_DIR) $(ISO_DIR)/boot/kernel.elf kernel.iso

run:
	@qemu-system-x86_64 -cdrom kernel.iso -m 2048M

debug:
	@qemu-system-x86_64 -cdrom kernel.iso -m 2048M -d int

.PHONY: all clean grub run 