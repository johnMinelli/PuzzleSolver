PuzzleSolver
============

This is a program that uses computer vision techniques to solve jigsaw puzzles using the shapes of the edges. It works best with fully-interlocking puzzles with rectangular pieces of approximately the same size.


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

On linux or mac run this from the PuzzleSolver directory (with the source files):

```
make
```

or

```
g++ -O3 `pkg-config --cflags opencv` -std=c++11 -o PuzzleSolver *.cpp `pkg-config --libs opencv`
```

This will result in an executable called PuzzleSolver.  Run it without any command line arguments and the usage information will be displayed.

# Using PuzzleSolver

It can be used on all pieces or a subset (i.e., the loose pieces of a partially completed puzzle).

1. Number the back of the puzzle pieces
2. Clean the glass to your flatbed scanner to remove dust, lint, and debris.
3. Place as many puzzle pieces face-up on a flatbed scanner as will fit, such that none of the pieces are touching the edges of the scan area, and not touching each other.  Leave the scanner lid open and turn off the lights when scanning... a black background in the resulting images is ideal. Repeat until all puzzle pieces have been scanned (consider making the scans at night).  Scan at a resolution of 200dpi.
4. When all puzzle pieces have been scanned, move the scanner images into a directory containing only the scanner images (this will be the PuzzleSolver "input directory").
5. Run PuzzleSolver, giving it the name of the input directory and the name of an output directory.  The output directory will be created if it doesn't already exist. Do not use the same directory for input and output.

estimated piece size
threshold
debug output



