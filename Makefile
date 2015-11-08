########################################################################
#                             Makefile                                 #
#                                                                      #
# Copyright (c) 2010 Autodesk Canada Co.                               #
# All rights reserved.                                                 #
#                                                                      #
# These coded instructions, statements, and computer programs          #
# contain unpublished proprietary information written by Autodesk      #
# Canada, and are protected by Federal copyright law. They may not     #
# be disclosed to third parties or copied or duplicated in any         #
# form, in whole or in part, without the prior written consent of      #
# Autodesk Canada.                                                     #
#                                                                      #
#                                                                      #
# This Makefile is designed to compile and link Spark code in order to #
# load sparks into the INFERNO/FLAME/FLINT/FLARE/SMOKE/BURN            #
# environment. Read the comments carefully and it should be very clear #
# how to compile and use Sparks.                                       #
#                                                                      #
########################################################################

SHELL = /bin/sh

#
# SPARK_OSNAME should be set to MACOSX or LINUX
#
ifndef SPARK_OSNAME
  SPARK_OSNAME = $(shell uname | tr "[:lower:]" "[:upper:]" | tr -d "[:digit:]" | sed 's/DARWIN/MACOSX/g')
endif

#
# Make sure to setenv OPENEXR_LOCATION to the location of half.h
#
ifdef OPENEXR_LOCATION
   INCL_OPENEXR = -I$(OPENEXR_LOCATION)
endif

#
# Flags used to compile on MACOSX
#
ifeq ($(SPARK_OSNAME),MACOSX)
  CXX = cc
  LD = cc
  CFLAGS = -fPIC \
    -DDL_LITTLE_ENDIAN \
    -D_DARWIN_USE_64_BIT_INODE \
    -arch x86_64 \
    $(INCL_OPENEXR) 
  LDFLAGS = \
    -arch x86_64 \
    -dynamiclib \
    -undefined dynamic_lookup
endif

#
# Flags used to compile on Linux
#
ifeq ($(SPARK_OSNAME),LINUX)
  LD = ld
  CXX = g++
  CFLAGS = -fPIC \
     -D_LARGEFILE64_SOURCE \
     -D_FILE_OFFSET_BITS=64 \
     $(INCL_OPENEXR)
  LDFLAGS = -shared \
     -Bsymbolic
  SPARK_HARDWARE = $(shell uname -i)
  ifeq ($(SPARK_HARDWARE), x86_64)
    PLATFORM_ABI = 64
  else
    PLATFORM_ABI = n32
  endif
endif


#
# .spark, .spark_x86_64
#
SPARK_EXTENSION = spark

ifeq ($(SPARK_OSNAME),LINUX)
  ifeq ($(PLATFORM_ABI), 64)
    SPARK_EXTENSION = spark_x86_64
  endif
endif

#
# The symbol SPARKSOURCE indicates where the source code for the Sparks
# resides. It is currently set to the local directory in which this
# Makefile resides. Should you decide to usr source code to a different
# directory, update this symbol accordingly.
#
SPARKSOURCE=.

#
# The symbol SPARKDESTINATION indicates the directory where the linked
# (i.e. loadable) sparks will be output. This directory should match
# the directory associated to the 'Spark' token in your configuration
# file so that FLAME/FLINT can locate the Sparks. It is assumed for the
# moment that this Makefile resides in the correct directory (hence the .)
#
# *** NOTE ***
# You must also create a 'setups' directory within the SPARKDESTINATION
# directory. This is where FLINT/FLAME will write Spark setup files that
# can be loaded for future use.
#
SPARKDESTINATION=.


#
# The SPARK_CFILES list indicates which Sparks are to be compiled. Any
# new Sparks that are developed should be added to this list. Please
# note that there must be a 'Tab' character as the first character on
# each line.
#
SPARK_CFILES = 

#
# sparkInvert_16fp depends on the half data type (a 16bit floating point 
# emulation) as defined in the openexr distribution which you can obtain
# from http://openexr.com.
#
SPARK_C++FILES = 3dlbaker.c
 
#
# Please don't touch these four lines
#
OFILES = $(SPARK_CFILES:.c=.o)
SFILES = $(SPARK_CFILES:.c=.$(SPARK_EXTENSION))

OFILES_C++ = $(SPARK_C++FILES:.c=.o)
SFILES_C++ = $(SPARK_C++FILES:.c=.$(SPARK_EXTENSION))

#
# The following script determines whether or not Sparks can be compiled
# under the operating system that your system is currently running. If
# you do not have IRIX 5.2 (or greater for future releases), then you must
# upgrade your system before Sparks can be used within your FLAME/FLINT
# environment.
#

sparks_target:    $(SFILES) $(SFILES_C++)

#
# These script lines perform the actual compilation and linking of the
# Sparks themselves. The creation of a Spark is actually twofold. The
# first step is to compile the '.c' sources into '.o' object code. The
# object code is then linked '-shared' to produce a '.spark' file that
# is the object that FLAME/FLINT loads. The loadable '.spark' file is
# what SGI terms a DSO (dynamic shared object). Please consult your SGI
# documentation for more info regarding shared objects (use 'man dso').
#
$(SFILES) : $(OFILES)
$(OFILES) : %.o : %.c
	@echo ""
	@echo "MAKING SPARK ..."
	cc $(CFLAGS) -c $(SPARKSOURCE)/$*.c -o $(SPARKSOURCE)/$*.o
	$(LD) $(LDFLAGS) $(SPARKSOURCE)/$*.o -o $(SPARKDESTINATION)/$*.$(SPARK_EXTENSION)

$(SFILES_C++) : $(OFILES_C++)
$(OFILES_C++) : %.o : %.c
	@echo ""
	@echo "MAKING SPARK ..."
	$(CXX) $(CFLAGS) -c $(SPARKSOURCE)/$*.c -o $(SPARKSOURCE)/$*.o
	$(CXX) $(LDFLAGS) $(SPARKSOURCE)/$*.o -o $(SPARKDESTINATION)/$*.$(SPARK_EXTENSION)
	@echo "DONE."
