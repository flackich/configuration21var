#!/bin/bash

SCRIPT=$(mktemp)

printf 'whoami\nabracadabra\nwhoami\n' > "$SCRIPT"

OUTPUT=$(./build/emulator \
  --vfs config/vfs.json \
  --script "$SCRIPT" 2>&1)

STATUS=$?

rm -f "$SCRIPT"

echo "$OUTPUT" | grep -q "неизвестная команда: abracadabra"

COUNT=$(echo "$OUTPUT" | grep -c "^kai4me$" || true)

if [ "$COUNT" -ne 1 ]; then
    echo "test_errors: FAILED"
    exit 1
fi

if [ "$STATUS" -eq 0 ]; then
    echo "test_errors: FAILED"
    exit 1
fi

echo "test_errors: OK"
