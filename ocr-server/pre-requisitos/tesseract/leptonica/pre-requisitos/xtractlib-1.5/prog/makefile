#/*====================================================================*
# -  Copyright (C) 2001 Leptonica.  All rights reserved.
# -  This software is distributed in the hope that it will be
# -  useful, but with NO WARRANTY OF ANY KIND.
# -  No author or distributor accepts responsibility to anyone for the
# -  consequences of using this software, or for whether it serves any
# -  particular purpose or works at all, unless he or she says so in
# -  writing.  Everyone is granted permission to copy, modify and
# -  redistribute this source code, for commercial or non-commercial
# -  purposes, with the following restrictions: (1) the origin of this
# -  source code must not be misrepresented; (2) modified versions must
# -  be plainly marked as such; and (3) this notice may not be removed
# -  or altered from any source or modified source distribution.
# *====================================================================*/

#   Makefile  (in xtract/prog for linux)
#
#   For a nodebug version:             make
#   For a debug version:               make DEBUG=yes
#   For a shared library version:      make SHARED=yes
#
#   To link and run programs using shared (dynamic linked) libraries,
#   you must do one of the following two things:
#
#     (a)  make sure your LD_LIBRARY_PATH variable points to the
#          directory in which the *.so files are placed, or
#     (b)  have the *.so files (or links to them) in this directory
# 
#


# Libraries are built into a binary tree with its root in the
# parent directory
ROOT_DIR = ..

BASE_LIB =	$(ROOT_DIR)/lib
LIB_NODEBUG =	$(ROOT_DIR)/lib/nodebug
LIB_DEBUG =	$(ROOT_DIR)/lib/debug
LIB_SHARED =	$(ROOT_DIR)/lib/shared

# Include files are found within the same tree
LOCAL_INCL =	$(ROOT_DIR)/src


CC =		gcc -ansi -D_BSD_SOURCE -DANSI -fPIC
#CC =            g++ -D_BSD_SOURCE -fPIC



ifdef  SHARED
    LIB =	$(LIB_SHARED)
    OPTIMIZE =	-O2
else
    ifdef  DEBUG
	LIB =	$(LIB_DEBUG)
	OPTIMIZE =	-g
    else
	LIB =	$(LIB_NODEBUG)
	OPTIMIZE =	-O2
    endif
endif

OPTIONS =

INCLUDES =	-I$(LOCAL_INCL) -I/usr/X11R6/include

CFLAGS =	$(OPTIMIZE) $(OPTIONS)
CPPFLAGS =      $(INCLUDES) -DINTEL_LINUX_ENV


LDFLAGS +=	-L$(LIB) -L/usr/lib


ifdef  SHARED
    XTRACTLIB =		$(LIB)/libxtract.so
else
    XTRACTLIB =		$(LIB)/libxtract.a
endif


BATCHLIBS =	$(XTRACTLIB) -lm

#########################################################################

SRC =		xtractprotos.c

######################################################################

all:
	make xtractprotos

######################################################################

xtractprotos:	xtractprotos.o $(BATCHLIBS)
	$(CC) -o xtractprotos xtractprotos.o $(BATCHLIBS)

###########################################################

clean:
	-@ for file in $(SRC:%.c=%) ; do \
		rm -f $$file; \
	done ;
	-@ for file in $(SRC:%.c=%.o) ; do \
		rm -f $$file; \
	done ;

###########################################################

depend:
	$(BIN)/makedepend -DNO_PROTOS $(CPPFLAGS) $(SRC)

dependprotos:
	$(BIN)/makedepend $(CPPFLAGS) $(SRC)

# DO NOT DELETE THIS LINE -- make depend depends on it.
