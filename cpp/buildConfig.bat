@call ../localProjectPath.bat
@call "%_PROJECT_PATH%/globalVariables.bat"

@if exist "%_PROJECT_PATH%/userSpecific/buildConfig.%_USER_SPECIFIC%.bat" (
	@call "%_PROJECT_PATH%/userSpecific/buildConfig.%_USER_SPECIFIC%.bat"
) else (
	@call "%_PROJECT_PATH%/userSpecific/buildConfig.default.bat"
)
@echo %_OPTIONS%

@if not exist generated (
	@mkdir "generated"
)
@cd generated

@if not exist %_USER_SPECIFIC% (
	@mkdir "%_USER_SPECIFIC%"
)
@cd %_USER_SPECIFIC%

@if not %_COMPILER_NAME% == "MinGW Makefiles" @(
	%_CMAKE% -G %_COMPILER_NAME% "../../" -DCMAKE_CONFIGURATION_TYPES="Debug;Release" %_OPTIONS%
) else @(
	@if not exist debug (
		@mkdir "debug"
	)
	@cd debug
	%_CMAKE% -G %_COMPILER_NAME% "../../" -DCMAKE_BUILD_TYPE="Debug" %_OPTIONS%
	@cd ..
	@if not exist release (
		@mkdir "release"
	)
	@cd release
	%_CMAKE% -G %_COMPILER_NAME% "../../" -DCMAKE_BUILD_TYPE="Release" %_OPTIONS%
)

@if "%1" neq "-nopause" (
	@pause
)