.PHONY: build clean run help install

# Build directory
BUILD_DIR := build
INSTALL_DIR := $(HOME)/.local/bin

help:
	@echo "Clear Vision Video Processor - Available targets:"
	@echo ""
	@echo "  make build       - Build the application"
	@echo "  make clean       - Clean build artifacts"
	@echo "  make install     - Install to ~/.local/bin"
	@echo "  make run         - Run with example (requires video file)"
	@echo "  make help        - Show this help message"
	@echo ""
	@echo "Usage example:"
	@echo "  ./video-processor video.mp4 -m model.onnx -n classes.txt -o output.mp4"

build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. && make
	@echo "Build complete! Binary: $(BUILD_DIR)/video-processor"

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build artifacts"

install: build
	@mkdir -p $(INSTALL_DIR)
	@cp $(BUILD_DIR)/video-processor $(INSTALL_DIR)/
	@chmod +x $(INSTALL_DIR)/video-processor
	@echo "Installed to $(INSTALL_DIR)/video-processor"

run: build
	@echo "Usage: $(BUILD_DIR)/video-processor <video_path> [options]"
	@echo ""
	@$(BUILD_DIR)/video-processor -h
