#!/bin/bash
APP_NAME="jacobi"
EXECUTABLE="run.sh"
TEST_DIRECTORY="tests/Experiments/jacobiExperimentsEUROPAR/"
NUMBER_ITERATIONS=30
NUMBER_INNER_ITERATIONS=10
NUMBER_CLUSTERS=16
NUMBER_PE=16
EXECUTION_TIMES=5

for INPUT_SIZE in 2048 4096 8192 12288
do
  for TILE_SIZE in 32 64 128
  do
    mkdir -p tests/Experiments/jacobiExperimentsEUROPAR/spec${INPUT_SIZE}${TILE_SIZE}/
    for i in `seq 1 ${EXECUTION_TIMES}`
    do
      echo "Running test..."
      echo "${INPUT_SIZE}_${TILE_SIZE}_${NUMBER_CLUSTERS}_${NUMBER_PE}_${i}"
      ./${EXECUTABLE} ${INPUT_SIZE} ${INPUT_SIZE} ${TILE_SIZE} ${TILE_SIZE} ${NUMBER_ITERATIONS} ${NUMBER_INNER_ITERATIONS} ${NUMBER_CLUSTERS} ${NUMBER_PE} | tee -a ${TEST_DIRECTORY}spec${INPUT_SIZE}${TILE_SIZE}/${APP_NAME}_test_${INPUT_SIZE}_${TILE_SIZE}_${NUMBER_CLUSTERS}_${NUMBER_PE}_${i}.txt
      sleep 1
    done
  done
done
