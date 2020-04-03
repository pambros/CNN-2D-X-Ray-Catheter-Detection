# Catheter segmentation in X-Ray fluoroscopy using convolutional neural networks

This code is the implementation of the method presented in the following paper:
	
- P. Ambrosini, D. Ruijters, W.J. Niessen, A. Moelker and T. van Walsum: [Fully Automatic and Real-Time Catheter Segmentation in X-Ray Fluoroscopy][2017Ambrosini]. The 20th International Conference on Medical Image Computing and Computer Assisted Intervention (MICCAI), Lecture Notes in Computer Science, vol. 10434, pp. 577-585, 2017.
arXiv preprint [![](docs/images/pdf.png)][2017AmbrosiniPreprint]

[2017Ambrosini]: https://doi.org/10.1007/978-3-319-66185-8_65
[2017AmbrosiniPreprint]: https://arxiv.org/abs/1707.05137

<p align="center">
	<img src="docs/images/catheterExtractionExample2.png">
</p>

## Neural network architecture

<p align="center">
	<img src="docs/images/diagramCNN.png">
</p>

## Prerequisites

Tested on:

- Linux Debian buster 10.3 with python 3.7.5, boost 1.68, cmake 3.13.4, gcc 8.3.0 and python libraries: tensorflow 2.2.0, numpy 1.18.2, pydicom 1.4.2, scipy 1.4.1, scikit-image 0.17dev0

- Windows 10 64bits with Python 3.6.1 and: (didn't test since tag 1.0 where Theano library was used, might not work anymore and should be updated)
	- Microsoft Visual C++ 14.00 2015 update 3 express version
	- Microsoft Visual C++ 12.00 2013 update 5 express version
	- MinGW-w64 and GCC 5.4.0

To build the C++ code you need:

- CMake to build the extraction curve library
- Boost, especially the compiled libboost_python and libboost_numpy, in order to call C++ function in python (e.g. on ubuntu/debian `apt-get install libboost-all-dev`)

To execute the python code you need the basic deep learning libraries:

- Tensorflow
- CUDA (tested with 10.1) and cuDNN (optional, tested with 7.6)

## Quick start

Note: For Windows use, in the following, replace all the sentences and commands with ".sh" file by ".bat" file.

First edit the file `userSpecific/globalVariables.default.sh` and change all the paths/parameters to fit your machine configuration (cuda, boost lib, python, cmake paths...). You can eventually modify as well `userSpecific/buildConfig.default.sh` but it should not be necessary.
Then, build the C++ project and python wrapper thanks to Boost.

```batch
cd cpp
buildConfigRedirect.sh
compileRedirect.sh
````

To see if everything went fine, check the files `cpp/generated/stderrbuildConfig.txt`, `cpp/generated/stdoutbuildConfig.txt`, `cpp/generated/stderrcompile.txt` and `cpp/generated/stdoutcompile.txt`.
You can also launch the debug python test.

```batch
cd ../tests/debugCppExtractCenterlineRedirect
DebugCppExtractCenterlineRedirect.sh
````

If you have "The library has been successfully compiled and imported!" in the file `generated/stdoutDebugCppExtractCenterline.txt`, it is good news.

Now you can execute the training/testing example. We do not provide 2D X-ray dataset so we propose to generate fake gaussian noise images with kind of catheter/guidewire visible on them as an example.
Execute the `GenerateTrainTestDataset` python code to create 20 dicom sequences (4 frames per sequence) with their associated centerline txt files.

```batch
cd ../../examples/generateTrainTestDataset
GenerateTrainTestDatasetRedirect.sh
````

You should have in the `generated` folder all the dicom sequences (`sequenceXXXX.dcm`) with the 2D centerline for each frame (`sequenceXXXXcenterlineYYYY.txt`).

We then train the neural network with the 10 first sequences and evaluate them with the 10 last sequences during 300 epochs.

```batch
cd ../trainCatheterSegmentationRedirect
TrainCatheterSegmentationRedirect.sh
````

The training lasts about ~15 min. The last output of Keras is something like this: `Epoch 300/300 5/5 - 2s - loss: -9.3999e-01 - DiceCoef: 0.9400 - val_loss: -8.9345e-01 - val_DiceCoef: 0.8935`. A 0.94 DICE coefficient for the training set and 0.89 for the evaluation set. The best weights (giving the best DICE) for the train set `generated/bestTrainWeight.h5` and the test set `generated/bestTestWeight.h5` are saved.

Finally after the training, we test the full catheter/guidewire extraction method (neural network, skeletonization and spline extraction).

```batch
cd ../testCatheterSegmentation
TestCatheterSegmentationRedirect.sh
````

In the folder `generated`, you should have 3 images for all the last images of the 20 sequences: the normal one, the output of the neural network and the overlay of the extracted centerline. On some images, we can see that the extraction method does not work when the catheter shape is a bit more complex even if the output of the neural network is quite ok. Hopefully these kind of shapes should be limited in real 2D catheterization X-ray fluoroscopies.

<p align="center">
	<img src="docs/images/catheterExtractionExample.png">
</p>
