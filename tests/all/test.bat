@setlocal
@echo ------------------------------------------------------------- %_USER_SPECIFIC%

@call "%_PROJECT_PATH%/globalVariables.bat"

@cd "%_PROJECT_PATH%/cpp"
@call buildConfig.bat -nopause > generated/stdoutbuildConfig.%_USER_SPECIFIC%.txt 2> generated/stderrbuildConfig.%_USER_SPECIFIC%.txt

@cd "%_PROJECT_PATH%/cpp"
@call compile.bat -nopause > generated/stdoutcompile.%_USER_SPECIFIC%.txt 2> generated/stderrcompile.%_USER_SPECIFIC%.txt

@cd "%_PROJECT_PATH%/tests/debugCppExtractCenterline"
@call DebugCppExtractCenterline.bat -nopause > generated/stdoutDebugCppExtractCenterline.%_USER_SPECIFIC%.txt 2> generated/stderrDebugCppExtractCenterline.%_USER_SPECIFIC%.txt

@cd "%_PROJECT_PATH%/examples/generateTrainTestDataset"
@call GenerateTrainTestDataset.bat -nopause > generated/stdoutGenerateTrainTestDataset.%_USER_SPECIFIC%.txt 2> generated/stderrGenerateTrainTestDataset.%_USER_SPECIFIC%.txt

@cd "%_PROJECT_PATH%/examples/trainCatheterSegmentation"
@call TrainCatheterSegmentation.bat -nopause > generated/stdoutTrainCatheterSegmentation.%_USER_SPECIFIC%.txt 2> generated/stderrTrainCatheterSegmentation.%_USER_SPECIFIC%.txt

@cd "%_PROJECT_PATH%/examples/testCatheterSegmentation"
@call TestCatheterSegmentation.bat -nopause > generated/stdoutTestCatheterSegmentation.%_USER_SPECIFIC%.txt 2> generated/stderrTestCatheterSegmentation.%_USER_SPECIFIC%.txt

@endlocal