#!/bin/bash
_PYTHON=python3

_CUDA_DIR=/usr/local/cuda

_CMAKE=cmake

# name of the compiler using cmake syntax
_COMPILER_NAME="Unix Makefiles"

_BOOST_SRC_DIR=/usr

export PATH=$_CUDA_DIR/bin:$PATH
export LD_LIBRARY_PATH=$_CUDA_DIR/lib64:$LD_LIBRARY_PATH

_TACE_BIN_DIR=$_PROJECT_PATH/cpp/generated/$_USER_SPECIFIC/release # for PyTACELib

export PYTHONPATH=$PYTHONPATH:$_PROJECT_PATH/python/common:$_TACE_BIN_DIR
