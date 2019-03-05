PuzzleSolver
============

This is a program that uses computer vision techniques to solve jigsaw puzzles using the shapes of the edges. It works best with fully-interlocking puzzles with rectangular pieces of approximately the same size.


# Installing OpenCV

OpenCV is required to run PuzzleSolver. This version compiles with OpenCV 3.x. Here is how I have done it on 2 different computers...

## OSX 
Tested on High Sierra
```
# Install Homebrew if you haven't already. Visit https://brew.sh and install it

# Install opencv
brew install opencv libomp
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
make
```

or

```
g++ -O3 `pkg-config --cflags opencv` -std=c++11 -o PuzzleSolver *.cpp `pkg-config --libs opencv`
```

The result will be an executable called PuzzleSolver.  Run it with the `--help` option and the usage information will be written to the console.

# Using PuzzleSolver

PuzzleSolver can be used on all pieces of a puzzle or a subset of pieces (i.e., the loose pieces of a partially completed puzzle).

1. Number the back of the puzzle pieces.
2. Clean the glass to your flatbed scanner to remove dust, lint, and debris.
3. Place as many puzzle pieces face-up on a flatbed scanner as will fit within the scan area, such that none of the pieces are touching the edges of the scan area, and not touching each other4.  Leave the scanner lid open and turn off the lights when scanning... a black background in the resulting images is ideal (consider making the scans at night if you can't get the room dark enough). Scan at 300dpi. Repeat until all puzzle pieces have been scanned once.
5. Move the scanner images into a directory containing only the scanner images (this will be the "input directory").
6. Run PuzzleSolver, giving it the name of the input directory and the name of an output directory.  The output directory will be created if it doesn't already exist. Do not use the same directory for input and output.
7. If you get warnings about corner quality, or not enough corners detected for a piece, try re-running with different values for `--estimated-size`, `--threshold`, `--corners-blocksize`, and/or `--filter`.  If you use the `--edit-corners` option, a GUI will be displayed for each piece where the corners quality is higher than the minimum corners quality threshold (`--corners-quality`).  The GUI allows the piece corner locations to be adjusted manually.  Try to reduce the number of corner issues using other options first before using `--edit-corners`.
8. For aditional troubleshooting, use the `--save-xxx` options to generate intermediate image files.
9. When you are ready solve the puzzle, add the --solve option.
10. Solutions are written to solution.txt, and if an image can be generated, solution.png.  The solution image will be displayed in a popup window if possible.  Press the 'r' key one or more times to rotate the image by 90 degrees each time.

## Tips

* Place the numbered pieces in order on the scanner glass.  After your first scan, run PuzzleSolver with the `--save-contours` option.  Look at the generated contours-001.png image and see if the numbers in that image match the order of your pieces in the input.  If not, you can tell PuzzleSolver how to order them using `--order <order>`, where "<order>" is one of lrtb, lrbt, rltb, rlbt, tbrl, tblr, btrl, or btlr.  The meaning of these is best explained by example... i.e., "lrtb" means number pieces "from left to right, going top to bottom." Keep in mind the order such as "lrtb" is the order of the pieces in the image -- it is related to, but not the same as the order on the scanner glass.

* The `--save-xxx` options tell PuzzleSolver to save intermediate image files to the output directory.  These files use a numbering convention where the first number in the filename is the input image number, the second number is the piece number within its input image, and the third number is the overall piece number among all input files.  Consider the value "003-005-061".  This is a piece ID in which "003" indicates that the piece comes from the third input image, the piece is the fifth piece in that image, and is the 61st piece among all input images.  The overall piece numbers are the ones that are written out to the solution.txt file.
