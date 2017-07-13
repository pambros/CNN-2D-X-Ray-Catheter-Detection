@call ../localProjectPath.bat
@call "%_PROJECT_PATH%/globalVariables.bat"

cd generated/%_USER_SPECIFIC%

@if not %_COMPILER_NAME% == "MinGW Makefiles" @(
	%_CMAKE% --build . --config Debug
	%_CMAKE% --build . --config Release
) else @(
	@cd debug
	%_CMAKE% --build .
	@cd ../release
	%_CMAKE% --build .
)

@if "%1" neq "-nopause" (
	@pause
)