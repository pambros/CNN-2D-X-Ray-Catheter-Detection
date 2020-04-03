#!/bin/bash
source ../../localProjectPath.sh
source $_PROJECT_PATH/globalVariables.sh
_PYTHON_FILE=`basename "$0"`
_PYTHON_FILE=${_PYTHON_FILE:0:-3}
_PWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

mkdir -p generated

$_PYTHON -u "$_PWD/$_PYTHON_FILE.py"

if [ "$1" != "-nopause" ]; then
	read -n1 -r -p "Press any key to continue..."
fi
