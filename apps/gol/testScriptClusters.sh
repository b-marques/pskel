#!/bin/bash
APP_NAME="gol"
EXECUTABLE="run.sh"
TEST_DIRECTORY="tests/golAPIExperiments/Scalability"
NUMBER_ITERATIONS=1
NUMBER_INNER_ITERATIONS=1
# NUMBER_CLUSTERS=16
NUMBER_PE=16
EXECUTION_TIMES=5

for INPUT_SIZE in 4096
do
  for TILE_SIZE in 128
  do
    for NUMBER_CLUSTERS in 1 2 4 6 8 10 12 14 16
    do
      for i in `seq 1 ${EXECUTION_TIMES}`
      do
        echo "Running test..."
        echo "${INPUT_SIZE}_${TILE_SIZE}_${NUMBER_CLUSTERS}_${NUMBER_PE}_${i}"
        ./${EXECUTABLE} ${INPUT_SIZE} ${INPUT_SIZE} ${TILE_SIZE} ${TILE_SIZE} ${NUMBER_ITERATIONS} ${NUMBER_INNER_ITERATIONS} ${NUMBER_CLUSTERS} ${NUMBER_PE} | tee -a ${TEST_DIRECTORY}/${APP_NAME}_test_${INPUT_SIZE}_${TILE_SIZE}_${NUMBER_CLUSTERS}_${NUMBER_PE}_${i}.txt
        sleep 1
      done
    done
  done
done
