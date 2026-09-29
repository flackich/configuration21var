#!/bin/bash
set -e

OUTPUT=$(printf 'exit\n' | ./build/emulator \
  --vfs example.json \
  --script config/startup.txt)

echo "$OUTPUT" | grep -q "VFS: example.json"
echo "$OUTPUT" | grep -q "Стартовый скрипт: config/startup.txt"
echo "$OUTPUT" | grep -q "ls: команда-заглушка"
echo "$OUTPUT" | grep -q "cd: команда-заглушка"

echo "test_config: OK"
