#!/usr/bin/env bash
set -e

# fixed parameters
ANGLES=(0 45 90)
DISTANCES=(1 5 10)
IMAGE_IDX=0
ATTEMPT=2
LMUL=8

# make sure result directory exists
mkdir -p "result${LMUL}"

# pre-create (and truncate) all output files
for d in "${DISTANCES[@]}"; do
  : > "result${LMUL}/test_distance_${d}.out"
done

# compile
gcc glcmSeq.c -o glcmSeq -lm
gcc glcmVec.c -march=rv64gcv -mabi=lp64d -mcmodel=medany -o glcmVec -lm

# run sequential version
for d in "${DISTANCES[@]}"; do
  OUTFILE="result${LMUL}/test_distance_${d}.out"
  echo "Running sequential, distance=$d → $OUTFILE"
  echo "Sequential: ${ANGLES[*]}" >> "$OUTFILE"
  echo "---------------------------------------------------" >> "$OUTFILE"
  for (( i=0; i<ATTEMPT; i++ )); do
  ./glcmSeq --distance "$d" --angles "${ANGLES[@]}" --normed 1 --image "$IMAGE_IDX" >> "$OUTFILE"
  sleep 1
  done
  echo "---------------------------------------------------" >> "$OUTFILE"
done

# run vector version, not optimized
for d in "${DISTANCES[@]}"; do
  OUTFILE="result${LMUL}/test_distance_${d}.out"
  echo "Running vector (optimized=0), distance=$d → $OUTFILE"
  echo "---------------------------------------------------" >> "$OUTFILE"
  echo "VectorizedNormal: ${ANGLES[*]}" >> "$OUTFILE"
  for (( i=0; i<ATTEMPT; i++ )); do
  ./glcmVec --distance "$d" --angles "${ANGLES[@]}" --normed 1 --optimized 0 --image "$IMAGE_IDX" --lmul "$LMUL" >> "$OUTFILE"
  sleep 1
  done
  echo "---------------------------------------------------" >> "$OUTFILE"
done

# run vector version, optimized
for d in "${DISTANCES[@]}"; do
  OUTFILE="result${LMUL}/test_distance_${d}.out"
  echo "Running vector (optimized=1), distance=$d → $OUTFILE"
  echo "---------------------------------------------------" >> "$OUTFILE"
  echo "VectorizedOpt: ${ANGLES[*]}" >> "$OUTFILE"
  for (( i=0; i<ATTEMPT; i++ )); do
  ./glcmVec --distance "$d" --angles "${ANGLES[@]}" --normed 1 --optimized 1 --image "$IMAGE_IDX" --lmul "$LMUL" >> "$OUTFILE"
  sleep 1
  done
  echo "---------------------------------------------------" >> "$OUTFILE"
done

