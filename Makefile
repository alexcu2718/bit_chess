BUILD_DIR := build
APP_TARGET := bit_chess_cpp_app
APP_PATH := $(BUILD_DIR)/app/$(APP_TARGET)

.PHONY: configure build run clean

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR) --target $(APP_TARGET)

run: build
	./$(APP_PATH)

clean:
	cmake --build $(BUILD_DIR) --target clean