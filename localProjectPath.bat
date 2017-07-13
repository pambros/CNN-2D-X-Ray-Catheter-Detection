@if "%_USER_SPECIFIC%" == "" @(
	@if exist "%~dp0/userSpecific/localProjectPath.userSpecific.bat" (
		@call "%~dp0/userSpecific/localProjectPath.userSpecific.bat"
	) else (
		@set _USER_SPECIFIC=default
	)
)
@if "%_PROJECT_PATH%" == "" @(
	@set _PROJECT_PATH=%~dp0
)