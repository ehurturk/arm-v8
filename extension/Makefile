.PHONY: all clean rebuild qemu hardware run_qemu run_hardware debug-monitor gdb

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

# Linker scripts
LINKER_HARDWARE = linker.ld
LINKER_QEMU = linker_qemu.ld

# Boot files
BOOT_HARDWARE = boot.S
BOOT_QEMU = boot_qemu.S

# For real rpi 3b hardware (32-bit mode with arm-none-eabi)
CFLAGS_HARDWARE = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7

# For QEMU emulation (optimized for QEMU's raspi2b)
CFLAGS_QEMU = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -mfpu=neon-vfpv4 -mfloat-abi=hard 
-mcpu=cortex-a7

# Common settings
CPPFLAGS = -Ifrontend -Ibackend/src

# Debug flags
CFLAGS_DEBUG = -Wall -g -O0 -nostdlib -nostartfiles -ffreestanding -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7-a -mcpu=cortex-a7

LDLIBS = -lc -lgcc -lm -lc

# Source files (C files are the same for both targets)
SRC_C = $(wildcard frontend/*.c) chess.c cstartup.c cstubs.c

# Object files for C sources
OBJ_C = $(SRC_C:.c=.o)

# Default target builds for hardware
all: hardware

# ========== HARDWARE TARGET ==========
hardware: kernel.img

# Hardware-specific object files
OBJ_HARDWARE = $(BOOT_HARDWARE:.S=.hardware.o) $(OBJ_C)

# Compile C files for hardware (default rule)
%.o: %.c
	$(CC) $(CFLAGS_HARDWARE) $(CPPFLAGS) $(LDLIBS) -c $< -o $@

# Compile hardware boot assembly
%.hardware.o: %.S
	$(AS) $< -o $@

# Link hardware kernel
kernel.elf: $(OBJ_HARDWARE)
	$(CC) $(CFLAGS_HARDWARE) $(OBJ_HARDWARE) -T $(LINKER_HARDWARE) $(LDLIBS) -o $@

# Create hardware binary
kernel.img: kernel.elf
	$(OBJCOPY) -O binary $< $@
	@echo "Hardware kernel built: $@ (size: $$(stat -c%s $@) bytes)"

# ========== QEMU TARGET ==========
qemu: kernel_qemu.img

# QEMU-specific object files  
OBJ_QEMU = $(BOOT_QEMU:.S=.qemu.o) $(SRC_C:.c=.qemu.o)

# Compile C files for QEMU
%.qemu.o: %.c
	$(CC) $(CFLAGS_QEMU) $(CPPFLAGS) $(LDLIBS) -c $< -o $@

# Compile QEMU boot assembly
%.qemu.o: %.S
	$(AS) $< -o $@

# Link QEMU kernel
kernel_qemu.elf: $(OBJ_QEMU)
	$(LD) $(OBJ_QEMU) -T $(LINKER_QEMU) -o $@

# Create QEMU binary
kernel_qemu.img: kernel_qemu.elf
	$(OBJCOPY) -O binary $< $@
	@echo "QEMU kernel built: $@ (size: $$(stat -f%z $@) bytes)"

# ========== QEMU EMULATION ==========
QEMU_RASPI_VER = raspi2b
QEMU_CPU_VER = cortex-a7

# Run QEMU target
run_qemu: qemu
	@echo "Emulating with QEMU..."
	@echo "[INFO]: Use Ctrl-A then x to terminate QEMU"
	qemu-system-arm -M $(QEMU_RASPI_VER) -cpu $(QEMU_CPU_VER) -nographic -kernel kernel_qemu.img

# Convenience alias
run: run_qemu

# Debug with QEMU monitor
debug-monitor: qemu
	@echo "QEMU Monitor commands:"
	@echo "  info registers   - show CPU registers"
	@echo "  x/10i 0x10000    - disassemble 10 instructions at 0x10000 (QEMU load address)"
	@echo "  x/4wx 0x8000     - examine 4 words at 0x8000"
	@echo "  info mtree       - show memory layout"
	@echo "  quit             - exit"
	qemu-system-arm -M $(QEMU_RASPI_VER) -cpu $(QEMU_CPU_VER) -kernel kernel_qemu.img -monitor stdio -nographic

# Debug with GDB
gdb: qemu
	@echo "Starting QEMU with GDB server..."
	@echo "Connect with: arm-none-eabi-gdb kernel_qemu.elf -ex 'target remote localhost:1234'"
	qemu-system-arm -M $(QEMU_RASPI_VER) -cpu $(QEMU_CPU_VER) -nographic -kernel kernel_qemu.img -s -S

# ========== HARDWARE DEPLOYMENT ==========
# Copy to SD card (adjust /dev/sdX to your SD card)
deploy_hardware: hardware
	@echo "WARNING: This will copy kernel.img to SD card boot partition"
	@echo "Make sure SD card is mounted and adjust the path below"
	@echo "sudo cp kernel.img /media/$$USER/boot/"

# ========== DEBUG BUILDS ==========
debug-hardware: clean
	@echo "Building debug version for hardware..."
	$(CC) $(CFLAGS_DEBUG) $(CPPFLAGS) -c chess.c -o chess.debug.o
	$(CC) $(CFLAGS_DEBUG) $(CPPFLAGS) -c cstartup.c -o cstartup.debug.o  
	$(CC) $(CFLAGS_DEBUG) $(CPPFLAGS) -c cstubs.c -o cstubs.debug.o
	$(CC) $(CFLAGS_DEBUG) $(CPPFLAGS) -c frontend/uart.c -o uart.debug.o
	$(AS) -g $(BOOT_HARDWARE) -o boot.debug.o
	$(LD) boot.debug.o chess.debug.o cstartup.debug.o cstubs.debug.o uart.debug.o -T $(LINKER_HARDWARE) -o kernel-debug.elf
	$(OBJCOPY) -O binary kernel-debug.elf kernel-debug.img
	@echo "Debug hardware build: kernel-debug.img"

debug-qemu: clean
	@echo "Building debug version for QEMU..."
	$(CC) $(CFLAGS_DEBUG) $(CPPFLAGS) -c chess.c -o chess.debug.o
	$(CC) $(CFLAGS_DEBUG) $(CPPFLAGS) -c cstartup.c -o cstartup.debug.o  
	$(CC) $(CFLAGS_DEBUG) $(CPPFLAGS) -c cstubs.c -o cstubs.debug.o
	$(CC) $(CFLAGS_DEBUG) $(CPPFLAGS) -c frontend/uart.c -o uart.debug.o
	$(AS) -g $(BOOT_QEMU) -o boot_qemu.debug.o
	$(LD) boot_qemu.debug.o chess.debug.o cstartup.debug.o cstubs.debug.o uart.debug.o -T $(LINKER_QEMU) -o kernel_qemu-debug.elf
	$(OBJCOPY) -O binary kernel_qemu-debug.elf kernel_qemu-debug.img
	@echo "Debug QEMU build: kernel_qemu-debug.img"

# ========== UTILITIES ==========
# Show build information
info:
	@echo "=== Build Targets ==="
	@echo "  hardware       - Build for real Raspberry Pi 3B (kernel.img)"
	@echo "  qemu           - Build for QEMU emulation (kernel_qemu.img)"
	@echo "  run            - Build and run in QEMU"
	@echo "  debug-hardware - Build debug version for hardware"
	@echo "  debug-qemu     - Build debug version for QEMU"
	@echo ""
	@echo "=== Current Configuration ==="
	@echo "  Hardware boot: $(BOOT_HARDWARE) -> $(LINKER_HARDWARE)"
	@echo "  QEMU boot:     $(BOOT_QEMU) -> $(LINKER_QEMU)"
	@echo "  C sources:     $(SRC_C)"

# Check what files exist
check:
	@echo "=== Required Files Check ==="
	@echo -n "Hardware boot ($(BOOT_HARDWARE)): "; [ -f $(BOOT_HARDWARE) ] && echo "✓ Found" || echo "✗ Missing"
	@echo -n "QEMU boot ($(BOOT_QEMU)): "; [ -f $(BOOT_QEMU) ] && echo "✓ Found" || echo "✗ Missing" 
	@echo -n "Hardware linker ($(LINKER_HARDWARE)): "; [ -f $(LINKER_HARDWARE) ] && echo "✓ Found" || echo "✗ Missing"
	@echo -n "QEMU linker ($(LINKER_QEMU)): "; [ -f $(LINKER_QEMU) ] && echo "✓ Found" || echo "✗ Missing"

clean:
	rm -f *.o *.hardware.o *.qemu.o *.debug.o
	rm -f frontend/*.o frontend/*.hardware.o frontend/*.qemu.o frontend/*.debug.o  
	rm -f backend/*.o backend/*.hardware.o backend/*.qemu.o backend/*.debug.o
	rm -f kernel.elf kernel_qemu.elf kernel-debug.elf kernel_qemu-debug.elf
	rm -f kernel.img kernel_qemu.img kernel-debug.img kernel_qemu-debug.img