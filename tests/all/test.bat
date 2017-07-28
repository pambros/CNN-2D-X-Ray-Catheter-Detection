@setlocal
@echo ------------------------------------------------------------- %_USER_SPECIFIC%

@call "%_PROJECT_PATH%/globalVariables.bat"

@cd "%_PROJECT_PATH%/cpp"
@call buildConfig.bat -nopause

@cd "%_PROJECT_PATH%/cpp"
@call compile.bat -nopause

@cd "%_PROJECT_PATH%/tests/debugCppExtractCenterline"
@call DebugCppExtractCenterline.bat -nopause

@endlocal