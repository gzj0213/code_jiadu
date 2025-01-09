#
#=====================================================================================
#
#      Filename:  Makefile
#
#   Description:  
#
#       Version:  1.0
#       Created:  2007年06月18日
#      Revision:  none
#      Compiler:  gcc
#
#        Author:  ZhouJian (), zhoujian@revenco.com
#       Company:  Goldsign
#
#=====================================================================================
#

#CC=gcc

CC=/home/kial/PLC/rk3588/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc
# set compiler argument
#-g to generate debug info for gdb
#CFLAGS=-Wall
#CFLAGS=-g -Wall
#CFLAGS=-Wall -O1
CFLAGS=-Wall -O2
#CFLAGS= -Wall -O1 -pipe -fomit-frame-pointer
#CFLAGS= -Wall  -march=pentium-mmx -O1 -pipe -fomit-frame-pointer
#CFLAGS=-O1
LDFLAGS=
SND_LDFLAGS=-lasound
AR=ar
RM=rm
ARFLAGS=-rc
RMFLAGS=-rf
TARGET=libmyfuns
ATARGET=$(TARGET).a
STARGET=$(TARGET).so
SHAREFLAGS=-fpic -shared
COPY=cp
COPYFLAG=-rf
INSTALL_DIR=/usr/lib/
INCLUDE_DIR=/usr/include/
LDCONFIG=ldconfig
MKDIR=mkdir
MKDIRFLAG=-p

all:	MYLIB	

install:
	$(MKDIR) $(MKDIRFLAG) $(INSTALL_DIR)
	$(COPY) $(COPYFLAG) *.a $(INSTALL_DIR)
	$(MKDIR) $(MKDIRFLAG) $(INCLUDE_DIR)
	$(COPY) $(COPYFLAG) *.h $(INCLUDE_DIR)
	$(LDCONFIG) $(INSTALL_DIR)

#uninstall:
	#$(RM) $(RMFLAGS) $(INSTALL_DIR)/$(STARGET)
	#$(RM) $(RMFLAGS) $(INSTALL_DIR)/$(ATARGET)
	#$(RM) $(RMFLAGS) $(INCLUDE_DIR)
	#$(LDCONFIG) $(INSTALL_DIR)

#MYLIB:		libconv.o		\
		#libdate_time.o	\
		#libttys.o \
		#libsound.o \
		#libiso14443a.o
		#$(AR) $(ARFLAGS) $(ATARGET) libconv.o libdate_time.o libttys.o libiso14443a.o
		#$(CC) $(SHAREFLAGS) -o $(STARGET) libconv.o libdate_time.o libttys.o libiso14443a.o

MYLIB:		libserial.a		\
		libiso14443a.a		\
		libiso14443hex.a	\
		libconv.a		\
		libtm.a			\
		libpriv_crypt.a		\
		libmisc.a		\
		libiso7816.a		\
		libace_lcd.a		\
		libaudio.a
		

libserial.a: libserial.o
	$(AR) $(ARFLAGS) libserial.a libserial.o

libiso14443a.a: libiso14443a.o
	$(AR) $(ARFLAGS) libiso14443a.a libiso14443a.o

libiso14443hex.a: libiso14443hex.o
	$(AR) $(ARFLAGS) libiso14443hex.a libiso14443hex.o

libconv.a: libconv.o
	$(AR) $(ARFLAGS) libconv.a libconv.o

libtm.a: libtm.o
	$(AR) $(ARFLAGS) libtm.a libtm.o

libiso7816.a: libiso7816.o
	$(AR) $(ARFLAGS) libiso7816.a libiso7816.o

libpriv_crypt.a:	libpriv_crypt.o
	$(AR) $(ARFLAGS) libpriv_crypt.a libpriv_crypt.o

libmisc.a:	libmisc.o
	$(AR) $(ARFLAGS) libmisc.a libmisc.o

libace_lcd.a:	libace_lcd.o
	$(AR) $(ARFLAGS) libace_lcd.a libace_lcd.o

libaudio.a:	libaudio.o
	$(AR) $(ARFLAGS) libaudio.a libaudio.o


libconv.o:	libconv.c		\
		libconv.h
		$(CC) -c libconv.c $(CFLAGS) 

libtm.o:	libtm.c	\
		libtm.h
		$(CC) -c libtm.c $(CFLAGS) 
		

libiso14443a.o: libiso14443a.c \
		libiso14443a.h
		$(CC) -c libiso14443a.c $(CFLAGS)

libiso14443hex.o:	libiso14443hex.c \
			libiso14443hex.h
			$(CC) -c libiso14443hex.c $(CFLAGS)

libiso7816.o:	libiso7816.c \
		libiso7816.h
		$(CC) -c libiso7816.c $(CFLAGS)


libserial.o:	libserial.c \
		libserial.h
		$(CC) -c libserial.c $(CFLAGS)


libpriv_crypt.o:libpriv_crypt.c \
		libpriv_crypt.h
		$(CC) -c libpriv_crypt.c $(CFLAGS)

libmisc.o:	libmisc.c \
		libmisc.h
		$(CC) -c libmisc.c $(CFLAGS)

libace_lcd.o:	libace_lcd.c \
		libace_lcd.h
		$(CC) -c libace_lcd.c $(CFLAGS)


libaudio.o:	libaudio.c \
		libaudio.h
		$(CC) -c libaudio.c $(CFLAGS)


clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *~

.PHONY: clean MYLIB all install uninstall
