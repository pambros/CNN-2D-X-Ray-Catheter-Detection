#!/bin/sh
if [ -z "$_GLOBAL_VARIABLES" ]; then
	if [ -f "${_PROJECT_PATH}/userSpecific/globalVariables.${_USER_SPECIFIC}.sh" ]; then
		source "${_PROJECT_PATH}/userSpecific/globalVariables.${_USER_SPECIFIC}.sh"
	else
		source "${_PROJECT_PATH}/userSpecific/globalVariables.default.sh"
	fi

	_GLOBAL_VARIABLES=defined
fi