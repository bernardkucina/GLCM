# Performance evaluation of a RISC-V vector processor for edge processing of Sensor Data

> **BSc thesis in the undergraduate program at the Faculty of Computer and Information Science, University of Ljubljana. The thesis is available online in the [UL Repository](https://repozitorij.uni-lj.si/IzpisGradiva.php?id=172692&lang=slv) and is written in Slovene.**  
> Mentor: doc. dr. Ratko Pilipović, Co-mentor: doc. dr. Octavian Mihai Machidon

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-green.svg)](LICENSE)
[![RISC-V RVV](https://img.shields.io/badge/RISC--V-RVV%201.0-blue)]()

---

## Abstract
In this thesis, we explored the open-source RISC-V Vector Extension (RVV), which enables SIMD-style operations and opens up new opportunities for more efficient data processing. As a case study, we implemented an algorithm for calculating the GLCM matrix and its associated features. The algorithm was adapted to run on a processor with RVV support and experimentally evaluated. The results demonstrated that the use of the vector extension provides a significant acceleration of computation compared to the basic sequential approach, while maintaining both the accuracy of the results and their applicability in further image analysis.

---

## Project overview
We have researched different algorithmic approaches for calculating GLCM and its features. For GLCM, we implemented versions with and without gather/scatter operations. For calculating GLCM features, we implemented an approach with and without tree addition. The optimal implementation is GLCM without gather/scatter operations and calculating GLCM features with tree addition.

**Main achievements:**
- First vectorized implementation of the GLCM algorithm on RVV 1.0.
- Achieved an average 1.52 speedup over the sequential implementation (LMUL = 8).
---
## Project stucture
```
.
├─ GLCM/
│  ├─ images/            # test images
│  ├─ glcmVec.c          # driver for vectorized version
│  ├─ glcmSeq.c          # driver for sequential version
│  ├─ glcmmath.h         # helper functions for calculating offset
│  ├─ glcmseq.h          # sequential implemetation of GLCM
|  ├─ glcmvec.h          # vectorized implemetation of GLCM
|  ├─ rvvconf.h          # helper functions for RVV configuration
|  ├─ stb_image_write.h
│  ├─ stb_image.h
|  ├─ setup.sh           # script for installing RISC-V tools
|  ├─ runPC.sh           # script for running tests on PC  
│  └─ run.sh             # script for running tests on OrangePi RV2
|
```

---

## Installation & Build
To run programs on your PC, install the [RISC-V ISA Simulator](https://github.com/riscv-software-src/riscv-isa-sim), the [RISC-V GNU Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain), and [RISC-V Proxy Kernel](https://github.com/riscv-software-src/riscv-pk). The following installation instructions were tested on Ubuntu 24.04 LTS. You can install the required tools with the `setup.sh` script. If you encounter any issues during installation, try executing the commands line by line.
```bash
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
```
---
## Running test
To run the tests on the OrangePi RV2, use `run.sh`. To run on a PC, use `runPC.sh`. Note that portability issues may occur on other platforms that support RVV 1.0.
```bash
# Command for running the script on the OrangePi RV2, where n is LMUL ∈ {1,2,4,8}.
bash run.sh n
# Command for running the script on PC, where n is LMUL ∈ {1,2,4,8}.
bash runPC.sh n
```
In both files `run.sh` and `runPC.sh` you can set te running test in following lines of code:

```bash
ANGLES=(0 45 90 135) # Array of angles for the test (in degrees)
DISTANCES=(5)        # Distance for GLCM calculation
IMAGE_IDX=0          # Image index (choose 0, 1, 2, or 3)
ATTEMPT=10           # Number of test runs
```
The following table reprisent image index for parameter `IMAGE_IDX`.
| IMAGE_IDX | Figure   | Filename                                                           |
|:-----------:|:----------:|:--------------------------------------------------------------------:|
| 0          | Figure 1 | `frame_0249_jpg.rf.d33720f1304d383c749e18cc08dea6db.jpg`           |
| 1          | Figure 2 | `frame_0055_jpg.rf.440b9803a9766de971934b5a891db20f.jpg`           |
| 2          | Figure 3 | `frame_0057_jpg.rf.88cee5058d48da760320392c6967ddde.jpg`           |
| 3          | Figure 4 | `frame_0252_jpg.rf.f685d90adb983dc8dafa4ae700342521.jpg`           |


---
## Results

Code was tested on the OrangePi RV2 platform, which supports RVV 1.0.  
The following table presents speedups for running the program on different images.

The figures are from the `images/` folder and are:
- **Figure 1** = `frame_0249_jpg.rf.d33720f1304d383c749e18cc08dea6db.jpg`
- **Figure 2** = `frame_0055_jpg.rf.440b9803a9766de971934b5a891db20f.jpg`
- **Figure 3** = `frame_0057_jpg.rf.88cee5058d48da760320392c6967ddde.jpg`
- **Figure 4** = `frame_0252_jpg.rf.f685d90adb983dc8dafa4ae700342521.jpg`

|                | Figure 1 | Figure 2 | Figure 3 | Figure 4 |
|----------------|:--------:|:--------:|:--------:|:--------:|
| **Speedup**    |   1.54   |   1.52   |   1.55   |   1.46   |

*Table: Speedups of the vectorized implementation of the GLCM algorithm and feature values across different images (see Figure 1–4 above), for the angle set `{0°, 45°, 90°, 135°}`, distance `d = 5`, and `LMUL = 8`.*

---
## Future work
We have found a faster solution, but there is still room for improvement. Currently, we compute the GLCM histogram in double-precision floating-point; switching to single precision could further increase speed and reduce memory usage at the cost of lower numerical accuracy. We also plan to explore unordered additive vector reduction, which is generally faster than ordered reduction, and to quantify the error introduced by this approach. The current implementation runs on a single core; we plan to implement a multicore version.

---

