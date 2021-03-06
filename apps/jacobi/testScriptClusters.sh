#!/bin/bash
APP_NAME="jacobi"
EXECUTABLE="run.sh"
TEST_DIRECTORY="tests/Experiments/jacobiExperimentsEUROPAR/"
NUMBER_ITERATIONS=30
NUMBER_INNER_ITERATIONS=10
# NUMBER_CLUSTERS=16
NUMBER_PE=16
EXECUTION_TIMES=5

for INPUT_SIZE in 4096
do
  for TILE_SIZE in 128
  do
    for NUMBER_CLUSTERS in 1 2 4 6 8 10 12 14 16
    do
      mkdir -p tests/Experiments/jacobiExperimentsEUROPAR/spec${NUMBER_CLUSTERS}/
      for i in `seq 1 ${EXECUTION_TIMES}`
      do
              echo "${INPUT_SIZE}_${TILE_SIZE}_${NUMBER_CLUSTERS}_${NUMBER_PE}_${i}"
              ./${EXECUTABLE} ${INPUT_SIZE} ${INPUT_SIZE} ${TILE_SIZE} ${TILE_SIZE} ${NUMBER_ITERATIONS} ${NUMBER_INNER_ITERATIONS} ${NUMBER_CLUSTERS} ${NUMBER_PE} | tee -a ${TEST_DIRECTORY}spec${NUMBER_CLUSTERS}/${APP_NAME}_test_${INPUT_SIZE}_${TILE_SIZE}_${NUMBER_CLUSTERS}_${NUMBER_PE}_${i}.txt

              sleep 1
      done
    done
  done
done
