@if not "%_GLOBAL_VARIABLES%" == "" @(
	goto _exit
)

@if exist "%~dp0/userSpecific/globalVariables.%_USER_SPECIFIC%.bat" (
	@call "%~dp0/userSpecific/globalVariables.%_USER_SPECIFIC%.bat"
) else (
	@call "%~dp0/userSpecific/globalVariables.default.bat"
)

@set _GLOBAL_VARIABLES=defined

:_exit