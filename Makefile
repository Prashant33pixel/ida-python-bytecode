# Python Bytecode Processor Module for IDA Pro
# Convenience Makefile

BUILD_DIR := build
CMAKE := cmake

# Detect IDA SDK
IDA_SDK_DIR ?= $(IDASDK)
ifndef IDA_SDK_DIR
    IDA_SDK_DIR := $(IDA_SDK)
endif

# Default target
.PHONY: all
all: build

# Configure and build
.PHONY: build
build: $(BUILD_DIR)/Makefile
	$(CMAKE) --build $(BUILD_DIR) --parallel

$(BUILD_DIR)/Makefile: CMakeLists.txt
	$(CMAKE) -B $(BUILD_DIR) -DIDA_SDK_DIR="$(IDA_SDK_DIR)"

# Clean build directory
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Full rebuild
.PHONY: rebuild
rebuild: clean build

# Install to IDA user directory
.PHONY: install
install: build
	$(CMAKE) --build $(BUILD_DIR) --target install_local

# Debug build
.PHONY: debug
debug: $(BUILD_DIR)/Makefile
	$(CMAKE) -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DIDA_SDK_DIR="$(IDA_SDK_DIR)"
	$(CMAKE) --build $(BUILD_DIR) --parallel

# Show configuration info
.PHONY: info
info:
	@echo "IDA SDK Directory: $(IDA_SDK_DIR)"
	@echo "Build Directory: $(BUILD_DIR)"
	@echo ""
	@echo "Targets:"
	@echo "  all/build  - Build the project"
	@echo "  clean      - Remove build directory"
	@echo "  rebuild    - Clean and rebuild"
	@echo "  install    - Install to ~/.idapro"
	@echo "  debug      - Build with debug symbols"
	@echo "  info       - Show this information"
