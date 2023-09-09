#!/bin/bash
set -xue

QEMU=qemu-system-riscv32

# clangのパス
CC=clang

# 格納場所
BUILD=./build
LINKER=./linker
KERNEL=./kernel

CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"

# $BUILDがない場合は作成
if [ ! -e $BUILD ]; then
    mkdir $BUILD
fi

# $LINKER/kernel.mapがない場合は作成
if [ ! -e $LINKER/kernel.map ]; then
    touch $LINKER/kernel.map
fi

# カーネルのビルド
$CC $CFLAGS -Wl,-T$LINKER/kernel.ld -Wl,-Map=$LINKER/kernel.map -o $BUILD/kernel.elf $KERNEL/kernel.c $KERNEL/common.c

# QEMUの起動
qemu-system-riscv32 -machine virt -bios default -nographic -serial mon:stdio -kernel $BUILD/kernel.elf
