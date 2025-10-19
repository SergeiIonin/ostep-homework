#!/bin/bash

# Test script for dining philosophers project
echo "=== Testing Dining Philosophers Project ==="
echo

# Show help
echo "1. Showing available make targets:"
make help
echo

# Clean and build
echo "2. Cleaning and building:"
make clean
make
echo

# Check if executable exists
if [ -f "./dining_philosophers" ]; then
    echo "3. Executable created successfully ✓"
    ls -la dining_philosophers
else
    echo "3. Build failed ✗"
    exit 1
fi

echo
echo "4. Running a quick test (5 seconds)..."
echo "   Note: On macOS, press Ctrl+C to stop after a few seconds"
echo

# Run the program (user should interrupt with Ctrl+C)
./dining_philosophers
