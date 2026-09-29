#!/bin/bash
set -e

OUTPUT=$(printf 'exit\n' | ./build/emulator \
  --vfs test_data/vfs_minimal.json \
  --script config/startup.txt 2>&1)

echo "$OUTPUT" | grep -q "VFS: test_data/vfs_minimal.json"
echo "$OUTPUT" | grep -q "Стартовый скрипт: config/startup.txt"

echo "test_config: OK"
