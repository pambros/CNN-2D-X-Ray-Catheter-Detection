#!/bin/bash
source ../localProjectPath.sh
source $_PROJECT_PATH/globalVariables.sh

cd generated/$_USER_SPECIFIC

cd debug
$_CMAKE --build .
cd ../release
$_CMAKE --build .

if [ "$1" != "-nopause" ]; then
	read -n1 -r -p "Press any key to continue..."
fi
