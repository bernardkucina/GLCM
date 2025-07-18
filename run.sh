#!/usr/bin/env bash
set -e

# fixed parameters
ANGLES=(0 45 90)
DISTANCES=(1 5 10)
IMAGE_IDX=0

# compile
gcc glcmSeq.c -o glcmSeq -lm
gcc glcmVec.c -march=rv64gcv -lp64d -mcmodel=medany -o glcmVec -lm
# COMPLE: ./glcmSeq --distance 1 --angles 0 45 90 --normed 1 --image 0 > test.out
# COMPLE: ./glcmVec --distance 1 --angles 0 45 90 --normed 1 --optimized 1 --image 0 > test.out

# run for each distance
for d in "${DISTANCES[@]}"; do
  OUTFILE="test_distance_${d}.out"
  echo "Running distance=$d → $OUTFILE"
  touch "./result/$OUTFILE" && rm "./result/$OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  ./glcmSeq --distance "$d" --angles "${ANGLES[@]}" --normed 1 --image "$IMAGE_IDX" >> "./result/$OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  sleep 1
done

for d in "${DISTANCES[@]}"; do
  OUTFILE="test_distance_${d}.out"
  echo "Running distance=$d → $OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  ./glcmVec --distance "$d" --angles "${ANGLES[@]}" --normed 1 --optimized 0 --image "$IMAGE_IDX" >> "./result/$OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  sleep 1
done

for d in "${DISTANCES[@]}"; do
  OUTFILE="test_distance_${d}.out"
  echo "Running distance=$d → $OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  ./glcmVec --distance "$d" --angles "${ANGLES[@]}" --normed 1 --optimized 1 --image "$IMAGE_IDX" >> "./result/$OUTFILE"
  echo "---------------------------------------------------" >> "./result/$OUTFILE"
  sleep 1
done
