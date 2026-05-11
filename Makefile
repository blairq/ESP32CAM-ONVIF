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

.PHONY: help build flash clean venv

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
	$(PIP) install platformio

venv: $(VENV)/bin/activate

## build: Build the firmware and copy to build/ directory
build: venv
	@echo "Building firmware..."
	$(PIO) run
	@mkdir -p build
	@echo "Copying firmware.bin to build/ ..."
	@find .pio/build -name "firmware.bin" -exec cp {} build/ \;
	@echo "Build complete. Binaries are in build/"

## flash: Flash the firmware to the device
flash: venv
	@echo "Flashing firmware..."
	$(PIO) run -t upload

## clean: Clean the build artifacts
clean: venv
	@echo "Cleaning project..."
	$(PIO) run -t clean
	rm -rf build
	rm -rf $(VENV)
