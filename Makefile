CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
TARGET = build/emulator
SOURCES = src/main.cpp src/Shell.cpp src/VirtualFileSystem.cpp

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: all
	./$(TARGET)

test: all
	./tests/test_basic.sh
	./tests/test_config.sh
	./tests/test_vfs.sh

clean:
	rm -rf build vfs_saved.json
