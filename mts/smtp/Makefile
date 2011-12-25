.POSIX:
#
# Makefile for mts/smtp subdirectory
#

SHELL = /bin/sh

top_srcdir = ../..
srcdir     = .


prefix      = /usr/local/nmh
exec_prefix = ${prefix}
bindir      = ${exec_prefix}/bin
libdir      = ${exec_prefix}/lib
etcdir      = ${prefix}/etc

CFLAGS   = -Wall -Wno-pointer-sign -O2
INCLUDES = -I../.. -I$(srcdir) -I$(top_srcdir) 
LINT	= echo 'No lint program found'
LINTFLAGS = 

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CFLAGS)

.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(COMPILE) $<

# header files
HDRS = smtp.h

# source
SRCS = hosts.c smtp.c

# object files in libsmtp.a
OBJS = hosts.o smtp.o

# auxiliary files
AUX = Makefile.in

# all files in this directory included in the distribution
DIST = $(HDRS) $(SRCS) $(AUX)

# ========= DEPENDENCIES FOR BUILDING AND INSTALLING ==========

all: $(OBJS)

install:

uninstall:

# ========== DEPENDENCIES FOR CLEANUP ==========

mostlyclean:
	rm -f *.o *~

clean: mostlyclean

distclean: clean
	#rm -f Makefile

realclean: distclean

superclean: realclean

# ========== DEPENDENCIES FOR LINT =================

lint: 
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(DEFS) $(SRCS)

# ========== DEPENDENCIES FOR MAINTENANCE ==========

subdir = mts/smtp

distdir = ../../`cat ../../distname`/$(subdir)
nmhdist: $(DIST)
	@echo "Copying distribution files in $(subdir)"
	@for file in $(DIST); do \
	  cp -p $(srcdir)/$$file $(distdir); \
	done

