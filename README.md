環境構築
sudo apt install -y clang llvm lld qemu-system-riscv32 curl
実行するディレクトリに移動
curl -LO https://github.com/qemu/qemu/raw/v8.0.4/pc-bios/opensbi-riscv32-generic-fw_dynamic.bin
./run.sh