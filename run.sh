#!/bin/bash
set -e

make
./build/emulator "$@"
