#!/bin/bash
_BATCH_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# echo $_BATCH_PATH

cd $_BATCH_PATH

_GENERATED_PATH=generated
mkdir -p $_GENERATED_PATH

_BATCH_NAME=`basename "$0"`
_BATCH_NAME=${_BATCH_NAME:0:-11}
# echo $_BATCH_NAME
./${_BATCH_NAME}.sh -nopause > generated/stdout${_BATCH_NAME}.txt 2> generated/stderr${_BATCH_NAME}.txt
