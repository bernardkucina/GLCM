#!/bin/bash
# Depenedancies for RISC-V ISA Simulator, RISC-V GNU Toolchain and RISC-V Proxy Kernel
sudo apt update
sudo apt install -y autoconf automake autotools-dev bc bison build-essential cmake curl device-tree-compiler flex gawk git gperf \
    libboost-all-dev libboost-regex-dev libboost-system-dev libexpat-dev libglib2.0-dev libgmp-dev libmpc-dev libmpfr-dev \
    libslirp-dev libtool ninja-build patchutils python3 python3-pip python3-tomli texinfo zlib1g-dev

# RISC-V ISA Simulator (spike)
# Using the /opt/riscv prefix is not recommended, as it can lead to write-permission issues.
mkdir $HOME/RISC-V
git clone https://github.com/riscv-software-src/riscv-isa-sim.git
cd riscv-isa-sim
git checkout 4196bc8fafadb643671a0d92a7b32ac84457dc8d
mkdir build
cd build
../configure --prefix=$HOME/RISC-V --with-isa=rv64gcv --with-target=riscv64-unknown-elf
make -j$(nproc)
make install
cd ../..
rm -rf riscv-isa-sim

# RISC-V GNU Toolchain
# Install the RISC-V GNU Toolchain first, as the riscv64-unknown-elf-gcc compiler is required to build the RISC-V Proxy Kernel and the Berkeley Boot Loader.
git clone https://github.com/riscv/riscv-gnu-toolchain.git
cd riscv-gnu-toolchain
git checkout 65cf14a6b36d48150619ccba0416c2a7d87f213b
./configure --prefix=$HOME/RISC-V --with-arch=rv64gcv --with-abi=lp64 --enable-multilib
make -j$(nproc)
cd ..
rm -rf riscv-gnu-toolchain

# Add the location of the riscv64-unknown-elf-gcc compiler to your PATH before building the RISC-V Proxy Kernel and the Berkeley Boot Loader (otherwise, you may encounter issues).
# EXAMPLE OF SUCH AN ISSUE:
# gcc: error: unrecognized argument in option ‘-mcmodel=medany’
# gcc: note: valid arguments to ‘-mcmodel=’ are: 32, kernel, large, medium, small
# make: *** [Makefile:332: file.o] Error 1
echo 'export PATH=$PATH:$HOME/RISC-V/bin' >> ~/.bashrc
echo 'export PATH=$PATH:$HOME/RISC-V/riscv64-unknown-elf/bin' >> ~/.bashrc
source ~/.bashrc

# RISC-V Proxy Kernel (pk)...
git clone https://github.com/riscv/riscv-pk.git
cd riscv-pk
mkdir build
cd build
../configure --prefix=$HOME/RISC-V --host=riscv64-unknown-elf
make -j$(nproc)  
make install
cd ../..
rm -rf riscv-pk