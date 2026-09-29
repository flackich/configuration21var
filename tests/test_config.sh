#!/bin/bash
set -e

OUTPUT=$(printf 'exit\n' | ./build/emulator \
  --vfs config/vfs.json \
  --script config/startup.txt)

echo "$OUTPUT" | grep -q "Стартовый скрипт: config/startup.txt"
echo "test_config: OK"
