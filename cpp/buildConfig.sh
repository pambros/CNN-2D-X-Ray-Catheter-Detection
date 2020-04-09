#!/bin/bash
source ../localProjectPath.sh
source $_PROJECT_PATH/globalVariables.sh
if [ -f "${_PROJECT_PATH}/userSpecific/buildConfig.${_USER_SPECIFIC}.sh" ]; then
	source "${_PROJECT_PATH}/userSpecific/buildConfig.${_USER_SPECIFIC}.sh"
else
	source "${_PROJECT_PATH}/userSpecific/buildConfig.default.sh"
fi
echo ${_OPTIONS}
echo `which python3`

mkdir -p generated/${_USER_SPECIFIC}
cd generated/${_USER_SPECIFIC}

mkdir -p debug
cd debug

$_CMAKE -G "$_COMPILER_NAME" "../../../" -DCMAKE_BUILD_TYPE=Debug $_OPTIONS

cd ..
mkdir -p release
cd release

# echo $_CMAKE -G "$_COMPILER_NAME" "../../../" -DCMAKE_BUILD_TYPE=Release $_OPTIONS
$_CMAKE -G "$_COMPILER_NAME" "../../../" -DCMAKE_INSTALL_PREFIX=$HOME/opt/ -DCMAKE_BUILD_TYPE=Release $_OPTIONS

if [ "$1" != "-nopause" ]; then
	read -n1 -r -p "Press any key to continue..."
fi
