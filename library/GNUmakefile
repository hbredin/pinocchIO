
# Include the common variables defined by the Makefile Package
include $(GNUSTEP_MAKEFILES)/common.make

LIBRARY_NAME = libpinocchIO
VERSION = 0.0.1

libpinocchIO_C_FILES =		\
	pIOAttributes.c		\
	pIODataset.c		\
	pIODatatype.c		\
	pIOFile.c		\
	pIORead.c		\
	pIOTimeComparison.c	\
	pIOTimeline.c		\
	pIOTypes.c		\
	pIOWrite.c		\
	structure_utils.c	
libpinocchIO_HEADER_FILES = 	\
	pinocchIO.h		\
	pIOAttributes.h		\
	pIODataset.h		\
	pIODatatype.h		\
	pIOFile.h		\
	pIOTypes.h		\
	pIOFile.h		\
	pIORead.h		\
	pIOTimeComparison.h	\
	pIOTimeline.h		\
	pIOWrite.h

libpinocchIO_LDFLAGS = -L/people/bredin/usr/local/lib -lhdf5 -lhdf5_hl
libpinocchIO_INCLUDE_DIRS = -I/people/bredin/usr/local/include

libpinocchIO_HEADER_FILES_INSTALL_DIR = pinocchIO

-include GNUmakefile.preamble

# Include in the rules for making GNUstep libraries
include $(GNUSTEP_MAKEFILES)/library.make

-include GNUmakefile.postamble
