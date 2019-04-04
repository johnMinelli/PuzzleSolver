PuzzleSolver
============

This is a program that uses computer vision techniques to solve jigsaw puzzles using the shapes of the edges. It works best with fully-interlocking puzzles with pieces of approximately the same size.  It can be used to solve puzzles from scratch, or help solve a partially completed puzzles.


# Installing OpenCV

OpenCV is required to run PuzzleSolver. This version compiles with OpenCV 3.x. Here is how I have done it on 2 different computers...

## OSX 
Tested on High Sierra with OpenCV 4.0.1
```
### Install Homebrew if you haven't already. Visit https://brew.sh and install it

### Install opencv
brew install opencv libomp
```

### Running on OSX
For some reason I have to set DYLD_FALLBACK_LIBRARY_PATH or PuzzleSolver fails to execute due to unresolved library issues.
```
DYLD_FALLBACK_LIBRARY_PATH=/usr/local/opt/lib ./PuzzleSolver ...
```
## Linux
Tested on Ubuntu 18.04
### Via apt-get
Installs OpenCV 3.2 as of Jan 2019

```
sudo apt-get install libopencv-dev
```

### Manual install

https://www.pyimagesearch.com/2018/05/28/ubuntu-18-04-how-to-install-opencv/

# Compiling PuzzleSolver

On Linux or Mac run this from the PuzzleSolver source code directory:
```
./build.sh
```

The result will be an executable called PuzzleSolver.  Run it with the `--help` option and the usage information will be written to the console.

## Enable source-level debugging

```
./configure CXXOPTS="-g -O0"
make clean && make
```

# Using PuzzleSolver

PuzzleSolver can be used on all pieces of a puzzle or a subset of pieces (i.e., the loose pieces of a partially completed puzzle).

The program offers two solution modes -- "automatic" and "guided".  Both modes require some some calibration via the command line arguments before entering the solution phase.  Automatic mode works well to solve an entire puzzle that has a small number of large pieces (about 100 pieces or less).  Guided solution mode is best for puzzles with large numbers of pieces whether or not you want to solve the puzzle from scratch or finish a partially completed puzzle.

* Separate any matched sets of pieces where the number of joined pieces is low (less than five).
* Number the back of the loose puzzle pieces sequentially starting with 1. 
* Clean the glass to your flatbed scanner to remove dust, lint, and debris.
* Place 20 to 30 puzzle pieces face-up on a flatbed scanner, but no more than will comfortably fit in the scan area, and make sure that none of the pieces are touching the edges of the scan area, and not touching each other.  For the loose pieces of a partially completed puzzle, or if you will be guided solution mode, order the pieces.
* Leave the scanner lid open and turn off the lights when scanning... a black background in the resulting images is ideal (consider making the scans at night if you can't get the room dark enough). Scan at 300dpi. Repeat until all puzzle pieces have been scanned once.
* Move the scanner images into a directory containing only the scanner images (this will be the "input directory").
* Run PuzzleSolver, giving it the name of the input directory and the name of an output directory.  The output directory will be created if it doesn't already exist. Do not use the same directory for input and output.
* If you get warnings about corner quality, or not enough corners detected for a piece, try re-running with different values for `--estimated-size`, `--threshold`, `--corners-blocksize`, `--median-blur-ksize`, and/or `--filter`.  If you use the `--edit-corners` option, a GUI will be displayed for each piece where the corners quality is higher than the minimum corners quality threshold (`--corners-quality`).  The GUI allows the piece corner locations to be adjusted manually.  Try to reduce the number of corner issues using other options first before using `--edit-corners`.
* For aditional troubleshooting, use the `--save-xxx` options to generate intermediate image files.
* When you are ready solve the puzzle, add the --solve option.
* Solutions are written to solution.txt, and if an image can be generated, solution.png.  The solution image will be displayed in a popup window if possible.  Press the 'r' key one or more times to rotate the image by 90 degrees each time.  Press the 'q' key to quit.

## Tips

* Place the numbered pieces in order on the scanner glass.  After your first scan, run PuzzleSolver with the `--save-contours` option.  Look at the generated contours-001.png image and see if the numbers in that image match the order of your pieces in the input.  If not, you can tell PuzzleSolver how to order them using `--order <order>`, where "<order>" is one of lrtb, lrbt, rltb, rlbt, tbrl, tblr, btrl, or btlr.  The meaning of these is best explained by example... i.e., "lrtb" means number pieces "from left to right, going top to bottom." Keep in mind the order such as "lrtb" is the order of the pieces in the image -- it is related to, but not the same as the order on the scanner glass.

* The `--save-xxx` options tell PuzzleSolver to save intermediate image files to the output directory.  These files use a numbering convention where the first number in the filename is the input image number, the second number is the piece number within its input image, and the third number is the overall piece number among all input files.  Consider the value "003-005-061".  This is a piece ID in which "003" indicates that the piece comes from the third input image, the piece is the fifth piece in that image, and is the 61st piece among all input images.  The overall piece numbers are the ones that are written out to the solution.txt file.
