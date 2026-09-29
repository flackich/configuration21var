CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
TARGET = build/emulator
SOURCES = src/main.cpp src/Shell.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -rf build
