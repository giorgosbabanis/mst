#!/bin/bash

export LL_INPUT_GRAPH_IS_SYMMETRIC=1
export LL_INPUT_GRAPH_PATH=data/test_csr_weighted_symmetric.txt
export WEIGHTED_DATA_GRAPH=1
export OMP_NUM_THREADS=12
./mst