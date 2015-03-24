CFLAGS = -Wall -Werror -g3 -g
CXXFLAGS = $(CFLAGS)
CPPFLAGS = "-std=c++0x"
LDLIBS = -lm
CC = g++ 

.PHONY: all clean

all: clean allocators allocators2

clean:
	rm -f allocators.o
	rm -f allocators2.o

allocators: allocators.o

allocators2: allocators2.o

allocators.o: allocators.cpp

allocators2.o: allocators2.cpp



