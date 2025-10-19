CC = gcc
CFLAGS = -Wall -Wextra -pthread -g -std=c99
TARGET = dining_philosophers
SOURCES = dining_philosophers.c
HEADERS = common_threads.h

# Default target
all: $(TARGET)

# Build the main program
$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) -o $(TARGET) $(SOURCES) $(CFLAGS)

# Clean up compiled files
clean:
	rm -f $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run with valgrind for memory checking
valgrind: $(TARGET)
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Run with helgrind for thread checking
helgrind: $(TARGET)
	valgrind --tool=helgrind ./$(TARGET)

# Debug build with extra debugging info
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Release build with optimizations
release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)

# Install dependencies (if needed)
install-deps:
	@echo "Installing development tools..."
	@echo "On Ubuntu/Debian: sudo apt-get install build-essential valgrind"
	@echo "On macOS: xcode-select --install"
	@echo "On Red Hat/CentOS: sudo yum groupinstall 'Development Tools'"

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build the program (default)"
	@echo "  clean     - Remove compiled files"
	@echo "  run       - Build and run the program"
	@echo "  valgrind  - Run with memory leak detection"
	@echo "  helgrind  - Run with thread race condition detection"
	@echo "  debug     - Build with debug symbols and no optimization"
	@echo "  release   - Build optimized release version"
	@echo "  help      - Show this help message"

.PHONY: all clean run valgrind helgrind debug release install-deps help
