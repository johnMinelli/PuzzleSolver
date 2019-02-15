PuzzleSolver
============

This is a program that uses computer vision techniques to solve jigsaw puzzles using the shapes of the edges.


# Installing OpenCV

OpenCV is required to run PuzzleSolver. This version compiles with OpenCV 3.x. Here is how I have done it on 2 different computers. 

## OSX (tested on High Sierra)

```
    # Install Homebrew if you haven't already. Visit https://brew.sh and install it

    # Install opencv
    brew install opencv libomp
```

## Linux (tested on Ubuntu 18.04)


### Via apt-get (installs OpenCV 3.2 as of Jan 2019)

```
sudo apt-get install libopencv-dev
```

### Manual install

https://www.pyimagesearch.com/2018/05/28/ubuntu-18-04-how-to-install-opencv/

# Compiling PuzzleSolver

Once OpenCV has been installed, you can try to open the xcodeproject, or on linux or mac run this from the PuzzleSolver directory (with the source files):

```
g++ -O3 `pkg-config --cflags opencv` -std=c++11 -o PuzzleSolver *.cpp `pkg-config --libs opencv`
```

This will result in an executable called PuzzleSolver.  Run it without any command line arguments and the usage information will be displayed.
