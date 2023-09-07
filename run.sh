#!/bin/bash
set -xue

qemu-system-riscv32 -machine virt -bios default -nographic -serial mon:stdio
