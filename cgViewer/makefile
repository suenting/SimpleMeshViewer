# Generic Makefile that should work with most programs you are to compile
#
# To compile and link your program all you have to do is run 'make' in the
# current directory. Make sure that all the source files (*.c) involved
# can be found in the current directory.
#
# To clean up object files run 'make clean_object'
#
# To delete any compiled files run 'make clean'

# Where to find the non standard library files
# INCLUDEDIR is where the header files can be found
# LIBDIR is where the library object files can be found

# Comment this out if it is not already in the search path
#INCLUDEDIR=/usr/local/include
LIBDIR=/usr/lib

# If you have more source files add them here 
SOURCE= MathUtil.h MathUtil.cpp Decimation.h Decimation.cpp pch.h main.cpp Model.h Model.cpp ModelFactory.h ModelFactory.cpp Subdivision.h Subdivision.cpp World.h World.cpp
# The compiler we're using 
CC= g++

# The flags that will be used to compile the object file. If you want
# the debug version of your program (i.e. you want to use a debugger), then
# you can add '-g' on the following line
CFLAGS= -O3 -Wall

# The basic library we're using. Add the other libraries you want to link
# to your program here.
CLIBFLAG= -lglut -lGLU -lGL -lglui

# If you have other library files in a different directory add them here.
# You may need to add '-L/usr/X11R6/lib'.
INCLUDEFLAG= -I. #-I$(INCLUDEDIR)
LIBFLAG= -L$(LIBDIR) -L/usr/X11R6/lib -lXi -lXmu

# Don't touch this one if you don't know what you're doing.
OBJECT= $(SOURCE:.C=.o)

# The name of the final executable 
EXECUTABLE=mcaq

# Don't touch any of these either if you don't know what you're doing.
all: $(OBJECT) depend
	$(CC) $(CFLAGS) $(OBJECT) $(INCLUDEFLAG) $(LIBFLAG) $(CLIBFLAG) -o $(EXECUTABLE)

depend:
	$(CC) -M $(SOURCE) > depend

$(OBJECT):
	$(CC) $(CFLAGS) -c -o $@ $(@:.o=.C)

clean_object:
	rm -f *.o

clean:
	rm -f depend $(EXECUTABLE) *.o

include depend
