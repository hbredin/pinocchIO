
# Include the common variables defined by the Makefile Package
include $(GNUSTEP_MAKEFILES)/common.make

TOOL_NAME = piols piodump

piols_C_FILES =   piols.c
piols_INCLUDE_DIRS = -I/people/bredin/usr/local/include
piols_LDFLAGS =      -L/people/bredin/usr/local/lib 

piodump_C_FILES =   piodump.c
piodump_INCLUDE_DIRS = -I/people/bredin/usr/local/include
piodump_LDFLAGS =      -L/people/bredin/usr/local/lib

-include GNUmakefile.preamble
include $(GNUSTEP_MAKEFILES)/tool.make
-include GNUmakefile.postamble
