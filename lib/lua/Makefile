.POSIX:

VPATH	= ../../contrib/lua-5.2.3/src

CFLAGS	= -DLUA_USE_POSIX
LDFLAGS	=
LIBS	= -lm

LIB	= ../liblua.a
SRCS	=	lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o \
		lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o \
		ltm.o lundump.o lvm.o lzio.o \
		lauxlib.o lbaselib.o lbitlib.o lcorolib.o ldblib.o liolib.o \
		lmathlib.o loslib.o lstrlib.o ltablib.o loadlib.o linit.o

OBJS	= $(SRCS:.c=.o)

all: $(LIB)

$(LIB): $(OBJS)
	ar -cr $(LIB) $(OBJS)

clean:
	rm -f $(LIB) $(OBJS)

install:

.PHONY: all clean install listsrcs
