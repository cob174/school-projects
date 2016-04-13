LIB     = /home/courses/cop4531p/LIB
CPP     = $(LIB)/cpp
TCPP    = $(LIB)/tcpp
AREA51  = $(LIB)/area51
PROJ    = .
INCPATH = -I$(PROJ) -I$(CPP) -I$(TCPP)

CC   = clang++ -std=c++11

VPATH = $(PROJ):$(CPP):$(TCPP)

all: frk.x

frk.x: frk.cpp rk.h
	$(CC) $(INCPATH) -ofrk.x frk.cpp

test: frk.x
	./auto_test.sh

