#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# PROJECT OPTIONS
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

PROGRAM=telescope
INCLUDE=-I./lib
MAN_PAGE=
BIN_PATH=/usr/local/bin
MAN_PATH=/usr/share/man


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# BUILD OPTIONS
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

COMPILER=gcc
#
#   optimize  enable all
#   level 3   warnings    include paths
#        \      |          /
CC_FLAGS=-O3 -Wall $(INCLUDE)
LD_FLAGS=-lm -lrt -pthread -lncursesw -lpanel -lGL -lGLU -lglut
#	  /     |      |        |           \
#      math     |     POSIX    ncurses with  ncurses
#           realtime  threads  wide-char     panels
#           extensions         support
#


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# FILES
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

SOURCES=src/main.c      \
        src/gl.c        \
        src/buffer.c    \
        src/nexstar.c   \
        src/serial.c



STATICS=lib/jdl/jdl.a
OBJECTS=$(SOURCES:.c=.o)


SOURCES_NC=src/ui_curses.c	\
        src/buffer.c    	\
        src/nexstar.c   	\
        src/serial.c


STATICS_NC=lib/jdl/jdl.a
OBJECTS_NC=$(SOURCES:.c=.o)


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# DEPENDENCIES
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

all: $(SOURCES)
	$(COMPILER) $(CC_FLAGS) $(SOURCES) $(STATICS) -o $(PROGRAM) $(LD_FLAGS)

#library:
	#make -C jlib

nc: $(SOURCES_NC)
	$(COMPILER) $(CC_FLAGS) $(SOURCES_NC) $(STATICS_NC) -o $(PROGRAM) $(LD_FLAGS)

install:
	install $(MAN_PAGE) $(MAN_PATH)/man1
	install $(PROGRAM) $(BIN_PATH)

clean:
	rm -f $(OBJECTS) $(PROGRAM) gmon.out

