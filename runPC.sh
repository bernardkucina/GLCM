#!/usr/bin/env bash
set -e

# fixed parameters
ANGLES=(0 45 90 135) # Array of angles for test
DISTANCES=(5)        # Distance for calculating GLCM
IMAGE_IDX=0          # Image index (choose 0,1,2 or 3)
ATTEMPT=10           # How many times the test is executed
LMUL=$1

# make sure result directory exists
mkdir -p "result${LMUL}"

# pre-create (and truncate) all output files
for d in "${DISTANCES[@]}"; do
  : > "result${LMUL}/test_distance_${d}.out"
done

# compile
riscv64-unknown-elf-gcc glcmSeq.c -o glcmSeq -lm
riscv64-unknown-elf-gcc glcmVec.c -march=rv64gcv -mabi=lp64d -mcmodel=medany -o glcmVec -lm

# run sequential version
for d in "${DISTANCES[@]}"; do
  OUTFILE="result${LMUL}/test_distance_${d}.out"
  echo "Running sequential, distance=$d → $OUTFILE"
  echo -e "Sequential: ${ANGLES[*]}\n" >> "$OUTFILE"
  for (( i=0; i<ATTEMPT; i++ )); do
  spike --varch=vlen:256,elen:64 --isa=rv64gcv pk ./glcmSeq --distance "$d" --angles "${ANGLES[@]}" --normed 1 --image "$IMAGE_IDX" >> "$OUTFILE"
  sleep 3
  done
  echo -e "\n" >> "$OUTFILE"
done

# run vector version, optimized
for d in "${DISTANCES[@]}"; do
  OUTFILE="result${LMUL}/test_distance_${d}.out"
  echo "Running vector (optimized=1), distance=$d → $OUTFILE"
  echo -e "VectorizedOpt: ${ANGLES[*]}\n" >> "$OUTFILE"
  for (( i=0; i<ATTEMPT; i++ )); do
  spike --varch=vlen:256,elen:64 --isa=rv64gcv pk ./glcmVec --distance "$d" --angles "${ANGLES[@]}" --normed 1 --optimized 1 --image "$IMAGE_IDX" --lmul "$LMUL" >> "$OUTFILE"
  sleep 3
  done
  echo -e "\n" >> "$OUTFILE"
done

# run vector version, not optimized
for d in "${DISTANCES[@]}"; do
  OUTFILE="result${LMUL}/test_distance_${d}.out"
  echo "Running vector (optimized=0), distance=$d → $OUTFILE"
  echo -e "VectorizedNormal: ${ANGLES[*]}\n" >> "$OUTFILE"
  for (( i=0; i<ATTEMPT; i++ )); do
  spike --varch=vlen:256,elen:64 --isa=rv64gcv pk ./glcmVec --distance "$d" --angles "${ANGLES[@]}" --normed 1 --optimized 0 --image "$IMAGE_IDX" --lmul "$LMUL" >> "$OUTFILE"
  sleep 3
  done
  echo -e "\n" >> "$OUTFILE"
done

