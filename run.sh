#!/bin/bash
set -xue

QEMU=qemu-system-riscv32

# 格納場所
BUILD=./build
OBJ=./obj
LINKER=./linker
KERNEL=./kernel
RISCV32=./riscv32

# .c,.h,.Sファイルの格納場所
SRCS_DIRS=("./kernel" "./sbi" "./libs" "./riscv32")

# ./OBJ/SRCS_DIRがない場合は作成
for SRCS_DIR in ${SRCS_DIRS[@]}; do
    if [ ! -e $OBJ/$SRCS_DIR ]; then
        mkdir $OBJ/$SRCS_DIR
    fi
done

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
qemu-system-riscv32 -m 128 -machine virt -bios default -nographic -serial mon:stdio -kernel $BUILD/kernel.elf
