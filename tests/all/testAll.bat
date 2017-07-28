@call ../../localProjectPath.bat

@for %%i in (%_USER_SPECIFIC_LIST%) do @(
	@set _USER_SPECIFIC=%%i
	@call test.bat
)