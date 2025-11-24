PROJECT_NAME := cv-skr
BUILD_DIR := build
TARGET := $(BUILD_DIR)/$(PROJECT_NAME)
SRC := src/main.cpp

CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -O2
PKG_CONFIG ?= pkg-config
OPENCV_PKG ?= opencv4
OPENCV_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(OPENCV_PKG) 2>/dev/null)
OPENCV_MODULES ?= opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs
OPENCV_LIBS := $(addprefix -l,$(OPENCV_MODULES))

.PHONY: all run clean

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(OPENCV_CFLAGS) $< -o $@ $(OPENCV_LIBS)

run: $(TARGET)
	./$(TARGET) data/entrada.png

clean:
	rm -rf $(BUILD_DIR)
