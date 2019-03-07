bin_PROGRAMS = PuzzleSolver
if USE_OPENCV4
OPENCV_CXXFLAGS = $(opencv4_CFLAGS)
OPENCV_LDDFLAGS = $(opencv4_LIBS)
else
OPENCV_CXXFLAGS = $(opencv_CFLAGS)
OPENCV_LDDFLAGS = $(opencv_LIBS)
endif
AM_CXXFLAGS = -std=c++11 $(OPENCV_CXXFLAGS) $(OPENMP_CFLAGS)
PuzzleSolver_LDADD = $(OPENCV_LDDFLAGS) $(OPENMP_CFLAGS)
PuzzleSolver_SOURCES = contours.cpp edge.cpp edit_corners.cpp main.cpp params.cpp piece.cpp puzzle.cpp PuzzleDisjointSet.cpp utils.cpp
