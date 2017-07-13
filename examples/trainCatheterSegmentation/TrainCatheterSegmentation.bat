@call ../../localProjectPath.bat
@call "%_PROJECT_PATH%/globalVariables.bat"
@set _PYTHON_FILE=%~n0
@set _PWD=%CD%
%_PYTHON% -u "%_PWD%/%_PYTHON_FILE%.py" %2

@if "%1" neq "-nopause" (
	@pause
)