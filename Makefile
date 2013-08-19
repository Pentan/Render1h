
FULL_OPTIMIZE = 1

ifdef FULL_OPTIMIZE
    CC = gcc
    CXX = g++
    OPTIMIZE = -O3 -fopenmp
else
    CC = cc
    CXX = c++
    OPTIMIZE = 
endif

CFLAGS = -Wall -g $(OPTIMIZE)
CXXFLAGS = -Wall -g $(OPTIMIZE)
LDFLAGS = -g -lstdc++ $(OPTIMIZE)

OBJS = \
	scene.o \
	render.o \
	mesh.o \
	main.o

#	scene.o \ \
#	xorshiftrandom.o \
#	vqmmath.o

TARGET = r1h01

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean :
	rm -f $(OBJS) $(TARGET)
