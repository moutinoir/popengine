CFLAGS = -Wall -Werror -g3 -g
CXXFLAGS = $(CFLAGS)
CPPFLAGS = "-std=c++0x"
LDLIBS = -lm
CC = g++ 

.PHONY: all clean

all: clean allocators

clean:
	rm -f allocators.o
	rm -f allocators

allocators: allocators.o

allocators.o: allocators.cpp

