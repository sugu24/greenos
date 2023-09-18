#!/bin/bash
set -xue

QEMU=qemu-system-riscv32

# 格納場所
BUILD=./build
LINKER=./linker
KERNEL=./kernel
RISCV32=./riscv32

# $BUILDがない場合は作成
if [ ! -e $BUILD ]; then
    mkdir $BUILD
fi

# $LINKER/kernel.mapがない場合は作成
if [ ! -e $LINKER/kernel.map ]; then
    touch $LINKER/kernel.map
fi

# カーネルのビルド
make

# QEMUの起動
qemu-system-riscv32 -machine virt -bios default -nographic -serial mon:stdio -kernel $BUILD/kernel.elf
