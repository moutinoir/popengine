CFLAGS = -Wall -Werror -g3 -g
CXXFLAGS = $(CFLAGS)
CPPFLAGS = "-std=c++0x"
LDLIBS = -lm
CC = g++ 

.PHONY: all clean

all: clean allocators3

clean:
	rm -f allocators3.o
	rm -f allocators3

allocators3: allocators3.o

allocators3.o: allocators3.cpp

