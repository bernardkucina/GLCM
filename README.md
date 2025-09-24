# Performance evaluation of a RISC-V vector processor for edge processing of Sensor Data

> **BCs thesis on undergraduate program on Faculty for computer and information science, University of Ljubljana**  
> Mentor: doc. dr. Ratko Pilipović, Co-mentor: doc. dr. Octavian Mihai Machidon

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](#licenca)
[![RISC-V RVV](https://img.shields.io/badge/RISC--V-RVV%201.0-blue)]()
[![Reproducible](https://img.shields.io/badge/reproducible-yes-success)]()

---

## Abstract
In this thesis, we explored the open-source RISC-V Vector Extension (RVV), which enables SIMD-style operations and opens up new opportunities for more efficient data processing. As a case study, we implemented an algorithm for calculating the GLCM matrix and its associated features. The algorithm was adapted to run on a processor with RVV support and experimentally evaluated. The results demonstrated that the use of the vector extension provides a significant acceleration of computation compared to the basic sequential approach, while maintaining both the accuracy of the results and their applicability in further image analysis.

---

## Main achevements
- First solution for vectorization of GLCM algoritm for RVV 1.0.
- Acheved on average 1.52 speedup in relation to sequential implementation.

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
│  └─ run.sh             # script for running test
|
```

---

## Zahteve / Orodja
- **Toolchain**: RISC-V GCC z RVV podporo (npr. `riscv64-unknown-elf-gcc` ali `riscv64-linux-gnu-gcc`)  
- **Knjižnice**: standardna C (brez odvisnosti)  
- **OS**: Linux na ciljni plošči (npr. OrangePi RV2)

---

## Namestitev & gradnja

```bash
# Na razvojni mašini:
git clone [[URL-do-repozitorija]].git
cd [[repo]]

# Navedi pravilni compiler v Makefile ali prek okolja:
export CC=riscv64-linux-gnu-gcc
export CFLAGS="-O3 -march=rv64gcv -mabi=lp64d"

make clean && make all   # izdela binarke: glcm_scalar, glcm_rvv

# Prenos na ploščo (primer):
scp bin/glcm_* user@board:/home/user/glcm/
```

---

## Reprodukcija meritev

```bash
# Na plošči:
cd ~/glcm
./glcm_scalar --images ./datasets/images --d 5 --angles 0,45,90,135 --repeat 5
./glcm_rvv    --images ./datasets/images --d 5 --angles 0,45,90,135 --repeat 5

# Po želji: merjenje porabe (odvisno od plošče/senzorjev):
sudo ./scripts/energy_readout.sh ./glcm_rvv ...
```

**Privzeti parametri**:
- razdalja `d = 5`  
- množice kotov `Θ₁={0°}`, `Θ₂={0°,45°}`, `Θ₃={0°,45°,90°}`, `Θ₄={0°,45°,90°,135°}`  
- sivinski nivoji: **[[npr. 8 / 16 / 32]]** (nastavljivo s `--levels`)

---

## Rezultati (primer)

| Implementacija | Θ                             | d | Povpr. čas [ms] | Speedup × | Opombe                |
|----------------|-------------------------------|---|------------------|-----------|-----------------------|
| Scalar         | {0°}                          | 5 | 12.8             | 1.00      | referenca             |
| RVV            | {0°}                          | 5 | 7.0              | **1.83**  | vektorska izvedba     |
| Scalar         | {0°,45°,90°,135°}            | 5 | 51.6             | 1.00      | referenca             |
| RVV            | {0°,45°,90°,135°}            | 5 | 28.2             | **1.83**  | omejitev: pasovna širina |

> **Opomba:** številke so **ilustrativne** — nadomesti jih z dejanskimi iz meritvenih skript.

**Značilke GLCM** (primer):
- Contrast ↑, Dissimilarity ↑, Homogeneity ↓, ASM/Energy ↔ (normalizacija)

Grafi (shrani jih v `results/plots/`):
- `čas_vs_theta.png`, `speedup_vs_levels.png`, `energy_vs_impl.png`

---

## Omejitve in prihodnje delo
- **Omejitve**: pasovna širina pomnilnika, razmerje med `VLEN` in dimenzijo GLCM, poravnava podatkov.  
- **Nadaljevanje**: optimizacija predpomnilnika/prednačrtovanja, **večnitenje** (SMT), razširitev na druge teksturne deskriptorje.

---

## Kako citirati

Če navajate to delo, uporabite (posodobi polja!):

```bibtex
@bachelors_thesis{Kucina2025GLCMRVV,
  author  = {Bernard Kučina},
  title   = {Ocena zmogljivosti RISC-V vektorskega procesorja za robno obdelavo senzorskih podatkov},
  school  = {[[Fakulteta / Univerza]]},
  year    = {2025},
  url     = {[[repo-ali-DOI]]}
}
```

---

## Licenca
Ta repozitorij je pod **MIT** licenco. Glej datoteko [`LICENSE`](LICENSE).

---

## Zahvale
Zahvaljujem se mentorju doc. dr. Ratku Pilipoviću in somentorju doc. dr. Octavianu Mihai Machidonu za usmeritve in podporo. Zahvala tudi skupnosti RISC-V za orodja in dokumentacijo.

---

## Kontakt
- Avtor: **Bernard Kučina** — [[e-pošta]]  
- Zadeve / hrošči: odprite *Issue* v tem repozitoriju

---

## (EN) Short abstract
We vectorize GLCM feature extraction on RISC-V RVV and benchmark it on an edge platform (e.g., OrangePi RV2). The repository contains scalar and RVV implementations, measurement scripts, and instructions to reproduce timing/energy results across angle sets and distances `d`.


