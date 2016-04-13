# makefile for sort tests

HOME = /home/courses/cop4531p/LIB
INC = -I. -I$(HOME)/cpp -I$(HOME)/tcpp
FLAGS = -Wall -Wextra 
CC = clang++ -std=c++11 $(FLAGS) $(INC)

all: part1 part2

part1: fgsort.x qsortDemo.x  hsortDemo.x

part2: ranuint.x sortspy.x

fgsort.x: gsort.h fgsort.cpp
	$(CC) -o fgsort.x fgsort.cpp

ranuint.x: ranuint.cpp
	$(CC) -o ranuint.x ranuint.cpp

sortspy.x: gsort.h sortspy.cpp
	$(CC) -o sortspy.x sortspy.cpp

qsortDemo.x: qsortDemo.cpp
	$(CC) -o qsortDemo.x qsortDemo.cpp

hsortDemo.x: hsortDemo.cpp
	$(CC) -o hsortDemo.x hsortDemo.cpp
