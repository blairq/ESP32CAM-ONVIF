UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    OS_NAME := Linux
endif
ifeq ($(UNAME_S),Darwin)
    OS_NAME := macOS
endif

PYTHON := python3
VENV := .venv
VENV_BIN := $(VENV)/bin
PIO := $(VENV_BIN)/pio
PIP := $(VENV_BIN)/pip
VENV_PYTHON := $(VENV_BIN)/python
MENUCONFIG := $(VENV_BIN)/menuconfig
ALLDEFCONFIG := $(VENV_BIN)/alldefconfig

.PHONY: help build flash clean venv menuconfig defconfig list-ports

.DEFAULT_GOAL := help

## help: Show available commands
help:
	@echo "Detected OS: $${OS_NAME:-$(UNAME_S)}"
	@echo "Available commands:"
	@grep -E '^## ' $(MAKEFILE_LIST) | sed -e 's/## //g' | awk -F': ' '{printf "  %-15s %s\n", $$1, $$2}'

$(VENV)/bin/activate:
	@echo "Creating virtual environment in $(VENV) on $${OS_NAME:-$(UNAME_S)}..."
	$(PYTHON) -m venv $(VENV)
	$(PIP) install --upgrade pip
	$(PIP) install platformio kconfiglib

venv: $(VENV)/bin/activate

## menuconfig: Run interactive configuration interface
menuconfig: venv
	@echo "Running menuconfig..."
	$(MENUCONFIG)
	@$(VENV_PYTHON) gen_config.py .config include/config.h

## defconfig: Generate default configuration
defconfig: venv
	@echo "Generating default configuration..."
	$(ALLDEFCONFIG)

## build: Build the firmware and copy to build/ directory
build: venv
	@echo "Building firmware..."
	@if [ ! -f .config ]; then \
		echo "No .config found, running defconfig..."; \
		$(MAKE) defconfig; \
		$(VENV_PYTHON) gen_config.py .config include/config.h; \
	fi
	$(PIO) run
	@mkdir -p build
	@echo "Copying firmware.bin to build/ ..."
	@find .pio/build -name "firmware.bin" -exec cp {} build/ \;
	@echo "Build complete. Binaries are in build/"

## list-ports: List available serial ports for flashing
list-ports: venv
	@echo "Available serial ports:"
	@$(PIO) device list

## flash: Flash the firmware to the device (optional PORT=/dev/ttyUSB0)
flash: venv
	@echo "Flashing firmware..."
	@if [ -z "$(PORT)" ]; then \
		echo "Tip: You can specify a port using 'make flash PORT=/dev/ttyUSB0'"; \
		echo "To list available ports, run 'make list-ports' or 'ls /dev/cu.*' (macOS) / 'ls /dev/ttyUSB*' (Linux)."; \
		echo "PlatformIO will attempt to auto-detect the port if none is provided."; \
		$(PIO) run -t upload; \
	else \
		echo "Using port: $(PORT)"; \
		$(PIO) run -t upload --upload-port $(PORT); \
	fi

## clean: Clean the build artifacts
clean: venv
	@echo "Cleaning project..."
	$(PIO) run -t clean
	rm -rf build
	rm -rf $(VENV)
	rm -f .config
	rm -f .config.old
