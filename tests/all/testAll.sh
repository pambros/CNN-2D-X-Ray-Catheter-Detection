#!/bin/bash
source ../../localProjectPath.sh

# clean all generated folders
cd $_PROJECT_PATH
find . -type d -name generated -prune -exec rm -rf {} \;

cd $_PROJECT_PATH/tests/all

for i in "${_USER_SPECIFIC_LIST[@]}"
do
	export _USER_SPECIFIC=$i
	./test.sh
done
