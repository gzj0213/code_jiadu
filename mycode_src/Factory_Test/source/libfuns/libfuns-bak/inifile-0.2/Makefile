srcdir = .

CC = gcc
AR = ar
SHELL = /bin/sh
CFLAGS = -fPIC
ARFLAGS = -rc

STATIC_LIBS = libinifile.a
SHARE_LIBS = libinifile.so

# The C code source files for this library. 
CSOURCES = $(srcdir)/inifile.c

# The header files for this library.
HSOURCES = $(srcdir)/inifile.h

OBJECTS = inifile.o

.c.o:
	rm -f $@
	$(CC) -c $(CCFLAGS) $<

##########################################################################

#all: static
all: static shared

static: $(OBJECTS)
	rm -f $(STATIC_LIBS)
	$(AR) $(ARFLAGS) $(STATIC_LIBS) $(OBJECTS)

shared: $(OBJECTS)
	${CC} -shared -o $@ $(OBJECTS)
#	-test -d shlib || mkdir shlib
#	-( cd shlib ; ${CC} -shared -o $@ $(OBJECTS) )

clean:
	rm -f $(OBJECTS) $(STATIC_LIBS) $(SHARE_LIBS)

inifile.o: inifile.c
