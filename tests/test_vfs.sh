#!/bin/bash
set -e

for FILE in \
  test_data/vfs_minimal.json \
  test_data/vfs_files.json \
  test_data/vfs_deep.json
do
    printf 'vfs-save /tmp/vfs_test.json\nexit\n' |
        ./build/emulator --vfs "$FILE" >/dev/null

    test -f /tmp/vfs_test.json
    rm -f /tmp/vfs_test.json
done

echo "test_vfs: OK"
