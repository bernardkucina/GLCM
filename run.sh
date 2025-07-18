#!/usr/bin/env bash
set -e

# fixed parameters
ANGLES=(0 45 90)
DISTANCES=(1 5 10)
IMAGE_IDX=0

# compile
riscv64-unknown-elf-gcc glcmSeq.c -o glcmSeq -lm
riscv64-unknown-elf-gcc glcmVec.c -o glcmVec -lm

# run for each distance
for d in "${DISTANCES[@]}"; do
  OUTFILE="test_distance_${d}.out"
  echo "Running distance=$d → $OUTFILE"
  touch "./result/$OUTFILE" && rm "./result/$OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  spike --varch=vlen:256,elen:64 --isa=rv64gcv pk ./glcmSeq --distance "$d" --angles "${ANGLES[@]}" --normed 1 --image "$IMAGE_IDX" >> "./result/$OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  sleep 1
done

for d in "${DISTANCES[@]}"; do
  OUTFILE="test_distance_${d}.out"
  echo "Running distance=$d → $OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  spike --varch=vlen:256,elen:64 --isa=rv64gcv pk ./glcmVec --distance "$d" --angles "${ANGLES[@]}" --normed 1 --optimized 0 --image "$IMAGE_IDX" >> "./result/$OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  sleep 1
done

for d in "${DISTANCES[@]}"; do
  OUTFILE="test_distance_${d}.out"
  echo "Running distance=$d → $OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  spike --varch=vlen:256,elen:64 --isa=rv64gcv pk ./glcmVec --distance "$d" --angles "${ANGLES[@]}" --normed 1 --optimized 1 --image "$IMAGE_IDX" >> "./result/$OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  sleep 1
done
