CXX=g++
CPPFLAGS=$(shell pkg-config --cflags opencv)
LDLIBS=$(shell pkg-config --libs opencv)

SRCS=$(shell ls *.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(OBJS)
	$(CXX) -g $(LDFLAGS) -o PuzzleSolver $(OBJS) $(LDLIBS)

%.o: %.cpp
	$(CXX) -g $(CPPFLAGS) -c $<

clean:
	rm -rf $(OBJS) PuzzleSolver
