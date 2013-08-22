
# FULL_OPTIMIZE = 1

ifdef FULL_OPTIMIZE
    CC = gcc
    CXX = g++
    EXTRAFLAGS = -O3 -fopenmp
else
    CC = cc
    CXX = c++
    EXTRAFLAGS = -g
endif

CFLAGS = -Wall $(EXTRAFLAGS)
CXXFLAGS = -Wall $(EXTRAFLAGS)
LDFLAGS = -lstdc++ $(EXTRAFLAGS)

OBJS = \
	scene.o \
	render.o \
	mesh.o \
	sphere.o \
	aabbgeometry.o \
	bvhnode.o \
	testscene.o \
	wavefrontobj.o

TESTOBJ = testmain.o
MAINOBJ = maincui.o

TESTTARGET = r1h01
MAINTARGET = raynbowd

# test
$(TESTTARGET) : $(TESTOBJ) $(OBJS)
	$(CC) -o $(TESTTARGET) $(TESTOBJ) $(OBJS) $(LDFLAGS)

# main
$(MAINTARGET) : $(MAINOBJ) $(OBJS)
	$(CC) -o $(MAINTARGET) $(MAINOBJ) $(OBJS) $(LDFLAGS)

clean :
	rm -f $(OBJS) $(TESTOBJ) $(MAINOBJ) $(TARGET)
