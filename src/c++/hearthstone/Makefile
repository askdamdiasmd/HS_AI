CC=g++

CFLAGS=-fPIC -Wall -std=c++11 -g #-D_DEBUG #-fopenmp -DUSE_OPENMP -O3 
LDFLAGS=-g -fPIC -Wall -fopenmp

SOURCES := $(shell find . -name '*.cpp')
OBJ := $(SOURCES:%.cpp=%.o)
HEADERS := $(shell find . -name '*.h')

all: hearthstone

%.o: %.cpp %.h
	$(CC) -o $@ $(CFLAGS) -c $<

hearthstone:  $(OBJ)
	g++ -o $@ $(LDFLAGS) $(OBJ)

clean:
	rm -f *.o hearthstone $(OBJ)

