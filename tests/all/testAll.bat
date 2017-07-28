@call ../../localProjectPath.bat

@if not exist generated (
	@mkdir "generated"
)

@for %%i in (%_USER_SPECIFIC_LIST%) do @(
	@set _USER_SPECIFIC=%%i
	@call test.bat
)