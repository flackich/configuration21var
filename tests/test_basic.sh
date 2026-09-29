#!/bin/bash
set -e

OUTPUT=$(printf 'whoami\nls\nexit\n' | ./build/emulator)

echo "$OUTPUT" | grep -q "Documents/"
echo "test_basic: OK"
