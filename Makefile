CC = g++
CFLAGS = -g -Wall
SRCS = main_0.cpp
PROG = main_0

OPENCV = `pkg-config opencv4 --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
	

