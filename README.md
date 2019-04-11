PuzzleSolver
============

This is a program that uses computer vision techniques to solve jigsaw puzzles using the shapes of the edges. 
It can be used to solve puzzles from scratch, or help solve partially completed puzzles. It works best with 
fully-interlocking puzzles that have pieces of approximately the same size.

PuzzleSolver is a hybrid command-line/GUI application.  Input parameters are specified on the command line. Status and 
results are written to the console and to various files in the output directory.  There are times when PuzzleSolver 
will pupup a GUI window -- namely when verifying contours found in the input images, when allowing the user to manually 
adjust the corner locations of a piece, and in guided solution mode to present suggested piece matches.  If a solution can 
be found, then an image of the solved puzzle is shown in a GUI window.


## Contents

- [Installation](#installing-opencv)
  - [Mac OSX](#osx)
  - [Linux](#linux)
- [Solve a built-in demo puzzle](#solving-a-demo-puzzle)
- [Solve your own puzzle](#solving-your-own-puzzle)
- [Command line options](#command-line-options-intro)
- [Solution Modes](#auto-solution-mode)
- [GUI Windows](#gui-windows)
  - [Contour verification](#contour-verification)
  - [Corner adjustment](#corner-adjustment)
  - [Guided match](#guided-match)
- [How It Works](#how-puzzlesolver-works)  


# Installing OpenCV

OpenCV is required to run PuzzleSolver. This version compiles with OpenCV version 2, 3, or 4. Here is how I have done it on 2 different computers...

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

## Source-level debugging
The default compiler optimizations can make it difficult to use source-level debugging.  To disable optimizatons, execute the following command.  PuzzleSolver will run noticably slower afterwards.
```
./configure CXXOPTS="-g -O0" && make clean && make
```
To re-enable the optimizations, do this:
```
./configure && make clean && make
```

# Using PuzzleSolver


## Solving a demo puzzle
PuzzleSolver includes scans for several demo puzzles.  A list of the demos appears at the end of the usage output when 
`-h` or `--help` is included in the command line options.  To run a demo, use the `--demo` option and give the demo puzzle name.
```
./PuzzleSolver --demo toy-story-color
```
When PuzzleSolver is done solving the puzzle, a popup window appears showing the completed puzzle.  The output directory for the demos is /tmp/${demo-puzzle--name}

## Solving your own puzzle

PuzzleSolver can be used on all pieces of a puzzle, or on just the loose pieces of a partially completed puzzle.

### Process Overview
- Number the back of the loose puzzle pieces in sequential order starting from "1". 
- Use a flatbed scanner and scan the backs of the pieces.  In each scan, you can include as many pieces as will fit such 
  that they don't touch each other or the edges of the scanning area.  Ideally, the background of the scanned images 
  should be black.  To achieve this, scan with the scanner lid open and the room lights off. Repeat until all pieces have been scanned.
- Place the images from the scanner in a directory by themselves (this will be referred to as the input directory).
- Run PuzzleSolver passing the pathname of the input directory and an output directory.  The output directory will be 
  created if it does not exist.  Adjust the command line options until all pieces are recognized and all warnings regarding 
  the quality of the piece corners have been mitigated (more on these subjects below)
- Run PuzzleSover again with the `--solve` or `--guided` option.  The `--solve` option invokes automatic solution mode, 
  good for solving complete puzzles with around 100 pieces or less.  The `--guided` option invokes an interactive mode 
  good for solving puzzles with a large number of pieces, or for solving partially completed puzzles.  For partially completed
  puzzles, PuzzleSolver will help you fit the loose pieces together via `--guided` solution mode.

Example input image:

![input image example](DocImages/pieces.png)

### Command line options intro

More detail on command line options is given in the [How PuzzleSolver Works](#how-puzzlesolver-works) section, below.

Usage synopsis:

```./PuzzleSolver <input directory> <output directory> [options]```

The input directory must only contain the scanned images of your puzzle pieces.  The output directory will be created if it doesn't exist.

Try starting with options `--estimated-piece-size 100` and `--verify-contours`.  When the contour window pops up, press 
the 't' key on your keyboard to toggle between viewing the numbered contours and the original color image.  Press the 'n' 
key to advance to the next image.  If contours do not show for some pieces, then try again with a lower estimated piece 
size value.  If too many contours are shown, then increase the estimated piece size.  If the assigned contour numbers are 
different compared to the input images, then read up on the `--order` option, below, or re-scan the pieces after adjusting 
their layout on the scanner bed.

If you get warnings about poor corners quality, try lowering the estimated piece size.  If that doesn't help, then try using 
a lower `--threshold` value.  For low DPI scans (200dpi or less) you might also need to reduce the value of `--corners-blocksize`. 
For persistent corner quality issues, use `--adjust-corners` to trigger a popup GUI window for each piece with poorly located corners. 
This window allows the corner locations to be adjusted -- click and drag the red circles until they are each positioned 
over a corner, then press the 'n' key to dismiss the window and advance to the next problematic piece, if any.  If no edits 
are necessary, press the 'n' key.  Manual adjustments are persisted in data files in the output directory.

When you are ready for PuzzleSolver to solve the puzzle, add the `--solve` or `--giuded` option.  In solution mode, PuzzleSolver 
computes a score for every possible edge-edge matchup.  The computed scores are used to determine how to fit peices together 
when finding the overall solution.  

#### Auto Solution Mode
In automatic mode (`--solve`), PuzzleSolver will attempt to solve the entire puzzle without any further input.  This works for the demos, and is likely to work for entire puzzles with low piece counts and relatively large phyiscal piece sizes.  Automatic solution mode, when successful, will show you an image of the completed puzzle.  The solution image file is also saved to the output directory as `solution.png`, and the console output is appended to `solution.log`.

#### Guided Solution Mode
In guided solution mode (`--guided`) you will be prompted via a popup GUI window to match pieces together.  Find the two pieces shown and check to see if they are a proper match -- if so, press the 'y' key on your keyboard, otherwise press the 'n' key.  For those pieces that fit to form a proper match, leave them connected.   With each 'y' or 'n' keypress, the GUI window will disappar and re-appear to prompt you to check another proposed match. As you progress, notice that the piece shown on the left side of this window will always part of the growing set of matched pieces, and that the peice shown on the right is from the collection of loose, unmatched pieces.  The 'y' and 'n' answers are rememberd in a data file in the output directory.  PuzzleSolver pick up from where it left of as long as it is invoked with the same command line parameters.

# GUI Windows

## Contour Verification

When the `--verify-contours` option is used, PuzzleSolver pops up a window showing the numbered piece contours found in the input images. 
With focus on this window, press the 't' key to toggle between viewing the contours and the original input image.  Press the 'n' key to advance
to the contours of the next input image.


![contour verification view](DocImages/contours.png)

## Corner Adjustment

When the `--adjust-corners` option is given then the corner adjustment GUI window is popped up for each piece with inadequate corner locations.
Using your mouse, drag the red circles so that the corner appears within the circle.  In the image below, the circle shown in the middle
right of the image needs to be dragged down to the corner of the piece below it.

The rendering scale of this window can be adjusted using the -/+ keys, or you can use the `--scale` option to set it for all popup windows.
Toggle between the color and black and white version of the piece with the 'c' key.  Press the 'n' key when you are done adjusting the corners, 
or if no adjustment is required.

If the corner locations are modified, then the updated locations are persisted to a data file in the output directory so that you won't have to re-adjust
them when re-running PuzzleSolver.  

| Before adjustment | After adjustment |
| :---: | :---: |
| ![before adjustment](DocImages/before-corner-adjustment.png) | ![after adjustment](DocImages/after-corner-adjustment.png) |

## Guided Match

In guided solution mode you will be prompted via a popup GUI window to accept or reject suggested piece matches.  The idea is
to actually try fitting the pieces together in the orientation shown to determine if they match or not.  Accept the match by 
pressing the 'y' key, and reject the match via the 'n' key.  If the pieces fit, leave them attached. Over time the size of the 
properly fitted, matched group of pieces will grow, and fewer unmatched pieces will be left.

PuzzleSolver attempts to find the best possible match of a new piece to the group of matched pieces you are currently 
working on.  It does this by finding the lowest edge-edge match score which includes an exterior edge in the group. 
Notice that the piece depicted on the left side of this window always belongs to the matched group, and the one on the 
right is from the set of unmatched pieces. The process is not perfect, especially for puzzles with small piece sizes, 
and often the suggested matches aren't the correct fit.  When a piece does fit, leave it attched and press the 'y' key. 

The rendering scale of this window can be adjusted using the -/+ keys, or you can use the `--scale` option to set it for all popup windows.


![guided match prompt](DocImages/guided-match-window.png)

TODO: work on a new group, set boundary edges.

# How PuzzleSolver works
This section introduces the internals of PuzzleSolver and its command line options. 
- The program begins by scanning the input directory for image files, sorts them by name into alphabetical order, and then loads them.
- For each image it finds, it identifies puzzle pieces within the image via a series of steps...
  - The image is filtered to reduce noise. By default, OpenCV's medianBlur() function is used.  Use the `--median-blur-ksize` command line option to override the default ksize value for the medianBlur() call.  Alternatively, a built-in filter function can be used instead of medianBlur() via the `--filter` option. 
  - A two color, black-and-white version of the image is created by first converting the image to greyscale, and then tresholding the image.  The threshold value can be controlled via the `--threshold` option.
  - OpenCV's findContours() method is used to find the piece contours in the black and white image.  Contours with a width or height less than the estimated piece size are assumed to be from image noise and are rejected.  Use `--estimated-piece-size` to override the default value.  This value is also important when detecting the piece corners later on.
  - The piece contours are sorted and numbered in the order they appear in the input image.  By understanding how PuzzleSolver numbers the pieces and by carefully arranging the numbered pieces on the glass when scanning, the piece numbers used in PuzzleSolver can be made to match those written on the peices.  The default numbering order is "left to right, top to bottom" (`lrtb`) within the image, as in the order of written words on a page.  The `--order` option can be used to change the default order value of `lrtb`. Eight different ordering options are available: lrtb, rltb, lrbt, rlbt, tblr, tbrl, btlr, and btrl.  To see what PuzzleSolver is doing here, use the `--verify-contours` option and the numbered contours will be displayed in a popup GUI window.  Press the 't' key when focus is on this window to toggle between the contours and the original color image.  Press the 'n' key to advance to the contours of the next image.
- For each puzzle piece that is found...
   - Small color and black and white images representing the piece are extracted from the the input image and associated with the piece data.
   - The locations of the piece corners are identified via an iterative process that calls OpenCV's goodFeaturesToTrack() function on the piece's black and white image.  Corner cobminations are rejected if the distance between corners is shorter than the value given by `--estimated-piece-size`.  The `blockSize` parameter passed to goodFeaturesToTrack() can be controlled via the `--corners-blocksize` option.
   - A check on the quality of the identified corner locations is performed by comparing the corners found to those of a rectangle.  The result is a 'corners quality' metric for which the value is higher for a piece with corner locations less like those of a rectangle, and lower for a piece where the corner locations are more like the corners of a rectangle.  If the value for a piece exceeds the `--corners-quality` option value, then a warning is reported to the console.  If a large number of warnings is issued, then the estimated piece size is probably set too high and should be lowered.  If warnings persist, then the corner locations for these pieces can be viewed and manually adjusted in a popup GUI window by re-running PuzzleSolver with the `--adjust-corners` option.  See below for more details on the corner adjustment GUI.
   - The piece contour is divided into four edge contours at the corner locations.  A copy of each edge contour is "normalized" -- i.e., translated and rotated so that one end is positioned at the origin and the other end is positioned above it on the y-axis.  This allows for easy automatic classification and comparison of edges within the software.
   - The shape of each edge is analysed and classified into one of three types: OUTER_EDGE, TAB, or HOLE. 
 - If you have not directed PuzzleSolver to proceed to the solution phase via `--solve`, `--guided`, or `--demo`, processing stop here and PuzzleSolver exits.
 - PuzzleSolver computes scores for each possible edge-edge combination.  Lower scores indicate a better match.  Impossible matches such as a TAB edge matched to another TAB edge are given the highest possible score.  Otherwise for every point in "this" contour the distances to the closest point in "that" contour are summed up and then added to the square of the difference in the distances between the two edge endpoints.
 - The edge-edge combinations and thier scores are sorted into ascending order by the score values.
 - In automatic mode, a solution is attempted by iterating down the sorted list, matching the two edges of each entry and rejecting the match if it results in an impossible physical arrangement of pieces such as overlaps, etc.  If the solution were to be visualized over time, it would appear as pieces randomly coalescing together until all pieces have been matched into a single group of pieces.
 - Guided solution mode is similar except that the the human operator participates in accepting and rejecting possible matches, and the number of matches sets is intentionally kept to a minimum for the sake of the user's sanity.


