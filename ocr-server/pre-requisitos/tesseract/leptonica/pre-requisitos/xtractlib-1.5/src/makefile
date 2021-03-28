#*====================================================================*
#-  Copyright (C) 2001 Leptonica.  All rights reserved.
#-  This software is distributed in the hope that it will be
#-  useful, but with NO WARRANTY OF ANY KIND.
#-  No author or distributor accepts responsibility to anyone for the
#-  consequences of using this software, or for whether it serves any
#-  particular purpose or works at all, unless he or she says so in
#-  writing.  Everyone is granted permission to copy, modify and
#-  redistribute this source code, for commercial or non-commercial
#-  purposes, with the following restrictions: (1) the origin of this
#-  source code must not be misrepresented; (2) modified versions must
#-  be plainly marked as such; and (3) this notice may not be removed
#-  or altered from any source or modified source distribution.
#*====================================================================*/


#   makefile (xtract)  (for linux)
#
#   For a nodebug version:             make
#   For a debug version:               make DEBUG=yes debug
#   For a shared library version:      make SHARED=yes shared
#   For all versions:                  make all
#
#   To remove all writes to stderr:    make -DNO_CONSOLE_IO
#
#   To remove object files: make clean
#
#   To generate function prototypes:  make allprotos
#
#

#   Stuff used by Makefile
RM =		rm -f
TEST =		test
MKDIRHIER =	mkdir -p
LIBRARIAN =	ar cq
RANLIB =	ranlib


# Libraries are built into a binary tree with its root in the parent directory
ROOT_DIR = ..

BASE_OBJ =	$(ROOT_DIR)/obj
OBJ_NODEBUG =	$(ROOT_DIR)/obj/nodebug
OBJ_DEBUG =	$(ROOT_DIR)/obj/debug
OBJ_SHARED =	$(ROOT_DIR)/obj/shared

BASE_LIB =	$(ROOT_DIR)/lib
LIB_NODEBUG =	$(ROOT_DIR)/lib/nodebug
LIB_DEBUG =	$(ROOT_DIR)/lib/debug
LIB_SHARED =	$(ROOT_DIR)/lib/shared


# Include files
INCLUDES =	-I./ -I/usr/X11R6/include -I/usr/include
PROTOTYPE_DIR =	.
CPPFLAGS =	$(INCLUDES) -DX86_LINUX_ENV


# shared library linker options
SONAME_OPTION = -Wl,-h,

ifdef  SHARED
    OPTIMIZE =		-O2 -fPIC
else
    ifdef  DEBUG
	OPTIMIZE =	-g
    else
	OPTIMIZE =	-O2
    endif
endif


CC =		gcc -ansi -D_BSD_SOURCE -DANSI -fPIC
#CC =		g++ -D_BSD_SOURCE -fPIC


LDFLAGSS +=		-L/usr/lib
OPTIONS =
CFLAGS =		$(OPTIMIZE) $(OPTIONS)
LIBRARIAN_SHARED =	gcc -shared

# Libraries differing only in their minor revision numbers
# are required to have the same interface.  By using
# "-h" in the ld, the "soname" is <libname>.X, where X is
# the major revision number.
# Links are created among the files <libname>.X.Y,
# <libname>.X, and <libname>, where Y is the minor revision number.
MAJOR_REV = 1
MINOR_REV = 5

#########################################################

# Libraries

XTRACTLIB =		libxtract.a

XTRACTLIB_SHARED =	libxtract.so

#########################################################

# Source

XTRACTLIB_C =	bytearray.c numabasic.c parseprotos.c sarray.c utils.c

SRC =		$(XTRACTLIB_C)


##################################################################

#  Main targets
 
nodebug: dirs $(XTRACTLIB:%=$(LIB_NODEBUG)/%)

all:
	make -f Makefile TARGET=$(TARGET) nodebug
	make -f Makefile TARGET=$(TARGET) DEBUG=true debug
	make -f Makefile TARGET=$(TARGET) SHARED=true shared

DEBUG_LIBS = $(LIB_DEBUG)/$(XTRACTLIB)
SHARED_LIBS = $(LIB_SHARED)/$(XTRACTLIB_SHARED)
debug:	dirs $(DEBUG_LIBS)
shared:	dirs $(SHARED_LIBS)


##################################################################

#   Proto targets

allprotos:	$(XTRACTLIB_C)
		../prog/xtractprotos -prestring=LEPT_DLL -protos=allprotos.h $(XTRACTLIB_C)

##################################################################

# Rule to make optimized libraries.

$(LIB_NODEBUG)/%.a:
		$(RM) $@
		$(LIBRARIAN) $@ $<
		$(RANLIB) $@

# Rule to make debuggable libraries.

$(LIB_DEBUG)/%.a:
		$(RM) $@
		$(LIBRARIAN) $@ $<
		$(RANLIB) $@

# Rule to make shared libraries.

$(LIB_SHARED)/%.so:
		$(RM) $@
		$(LIBRARIAN_SHARED) $(SONAME_OPTION)$(notdir $@).$(MAJOR_REV) -o $@ $<
	mv $@ $@.$(MAJOR_REV).$(MINOR_REV)
	cd $(LIB_SHARED); rm $(notdir $@).$(MAJOR_REV); \
	  ln -s $(notdir $@).$(MAJOR_REV).$(MINOR_REV) $(notdir $@).$(MAJOR_REV)
	cd $(LIB_SHARED); rm $(notdir $@); \
	  ln -s $(notdir $@).$(MAJOR_REV) $(notdir $@)



##################################################################

# library dependencies and rules

xtractlib:	$(LIB_NODEBUG)/$(XTRACTLIB)
$(LIB_NODEBUG)/$(XTRACTLIB):	$(XTRACTLIB_C:%.c=$(OBJ_NODEBUG)/%.o)
		$(RM) $@
		$(LIBRARIAN) $@ $(XTRACTLIB_C:%.c=$(OBJ_NODEBUG)/%.o)
		$(RANLIB) $@

xtractlibd:	$(LIB_DEBUG)/$(XTRACTLIB)
$(LIB_DEBUG)/$(XTRACTLIB):	$(XTRACTLIB_C:%.c=$(OBJ_DEBUG)/%.o)
		$(RM) $@
		$(LIBRARIAN) $@ $(XTRACTLIB_C:%.c=$(OBJ_DEBUG)/%.o)
		$(RANLIB) $@

xtractlibs:	$(LIB_SHARED)/$(XTRACTLIB_SHARED)
$(LIB_SHARED)/$(XTRACTLIB_SHARED):	$(XTRACTLIB_C:%.c=$(OBJ_SHARED)/%.o)
		$(RM) $@
		$(LIBRARIAN_SHARED) $(SONAME_OPTION)$(notdir $@).$(MAJOR_REV) -o $@ $(XTRACTLIB_C:%.c=$(OBJ_SHARED)/%.o)
	mv $@ $@.$(MAJOR_REV).$(MINOR_REV)
	cd $(LIB_SHARED); rm $(notdir $@).$(MAJOR_REV); \
	  ln -s $(notdir $@).$(MAJOR_REV).$(MINOR_REV) $(notdir $@).$(MAJOR_REV)
	cd $(LIB_SHARED); rm $(notdir $@); \
	  ln -s $(notdir $@).$(MAJOR_REV) $(notdir $@)

#########################################################

#  Rules for compiling source

$(OBJ_NODEBUG)/%.o:	%.c
		@$(TEST) -d $(OBJ_NODEBUG) || $(MKDIRHIER) $(OBJ_NODEBUG)
		$(COMPILE.c) -o $@ $<

$(OBJ_DEBUG)/%.o:	%.c
		@$(TEST) -d $(OBJ_DEBUG) || $(MKDIRHIER) $(OBJ_DEBUG)
		$(COMPILE.c) -o $@ $<

$(OBJ_SHARED)/%.o:	%.c
		@$(TEST) -d $(OBJ_SHARED) || $(MKDIRHIER) $(OBJ_SHARED)
		$(COMPILE.c) -o $@ $<

###########################################################

# Prepare a local environment

dirs:
		@$(TEST) -d $(BASE_OBJ) || $(MKDIRHIER) $(BASE_OBJ)
		@$(TEST) -d $(OBJ_NODEBUG) || $(MKDIRHIER) $(OBJ_NODEBUG)
		@$(TEST) -d $(OBJ_DEBUG) || $(MKDIRHIER) $(OBJ_DEBUG)
		@$(TEST) -d $(OBJ_SHARED) || $(MKDIRHIER) $(OBJ_SHARED)
		@$(TEST) -d $(BASE_LIB) || $(MKDIRHIER) $(BASE_LIB)
		@$(TEST) -d $(LIB_NODEBUG) || $(MKDIRHIER) $(LIB_NODEBUG)
		@$(TEST) -d $(LIB_DEBUG) || $(MKDIRHIER) $(LIB_DEBUG)
		@$(TEST) -d $(LIB_SHARED) || $(MKDIRHIER) $(LIB_SHARED)


###########################################################

clean:		
		$(RM) $(OBJ_NODEBUG)/*.o $(OBJ_DEBUG)/*.o \
			$(OBJ_SHARED)/*.o \
			$(LIB_NODEBUG)/*.a $(LIB_DEBUG)/*.a \
			$(LIB_SHARED)/*.so $(LIB_SHARED)/*.so.? \
			$(LIB_SHARED)/*.so.?.?

###########################################################

