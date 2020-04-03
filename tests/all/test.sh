echo ------------------------------------------------------------- $_USER_SPECIFIC

source $_PROJECT_PATH/globalVariables.sh

cd $_PROJECT_PATH/cpp
mkdir -p generated
./buildConfig.sh -nopause > generated/stdoutbuildConfig.$_USER_SPECIFIC.txt 2> generated/stderrbuildConfig.$_USER_SPECIFIC.txt

cd $_PROJECT_PATH/cpp
mkdir -p generated
./compile.sh -nopause > generated/stdoutcompile.$_USER_SPECIFIC.txt 2> generated/stderrcompile.$_USER_SPECIFIC.txt

cd $_PROJECT_PATH/tests/debugCppExtractCenterline
mkdir -p generated
./DebugCppExtractCenterline.sh -nopause > generated/stdoutDebugCppExtractCenterline.$_USER_SPECIFIC.txt 2> generated/stderrDebugCppExtractCenterline.$_USER_SPECIFIC.txt

cd $_PROJECT_PATH/examples/generateTrainTestDataset
mkdir -p generated
./GenerateTrainTestDataset.sh -nopause > generated/stdoutGenerateTrainTestDataset.$_USER_SPECIFIC.txt 2> generated/stderrGenerateTrainTestDataset.$_USER_SPECIFIC.txt

cd $_PROJECT_PATH/examples/trainCatheterSegmentation
mkdir -p generated
./TrainCatheterSegmentation.sh -nopause > generated/stdoutTrainCatheterSegmentation.$_USER_SPECIFIC.txt 2> generated/stderrTrainCatheterSegmentation.$_USER_SPECIFIC.txt

cd $_PROJECT_PATH/examples/testCatheterSegmentation
mkdir -p generated
./TestCatheterSegmentation.sh -nopause > generated/stdoutTestCatheterSegmentation.$_USER_SPECIFIC.txt 2> generated/stderrTestCatheterSegmentation.$_USER_SPECIFIC.txt
