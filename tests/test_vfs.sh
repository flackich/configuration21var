#!/bin/bash
set -e

for FILE in \
  test_data/vfs_minimal.json \
  test_data/vfs_files.json \
  test_data/vfs_deep.json
do
  printf 'exit\n' | ./build/emulator --vfs "$FILE" >/dev/null
done

echo "test_vfs: OK"
