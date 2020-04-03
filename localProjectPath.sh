#!/bin/sh
if [ -z "$_PROJECT_PATH" ]; then
	export _PROJECT_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi
# echo $_PROJECT_PATH
if [ -z "$_USER_SPECIFIC" ]; then
	if [ -f "${_PROJECT_PATH}/userSpecific/localProjectPath.userSpecific.sh" ]; then
		source "${_PROJECT_PATH}/userSpecific/localProjectPath.userSpecific.sh"
	else
		export _USER_SPECIFIC=default
	fi
fi
# echo $_USER_SPECIFIC
