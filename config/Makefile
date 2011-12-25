.POSIX:
#
# Makefile for config subdirectory
#

# nmh version
VERSION = posix-dev

SHELL = /bin/sh

top_srcdir = ..
srcdir     = .


prefix      = /usr/local/nmh
exec_prefix = ${prefix}
bindir      = ${exec_prefix}/bin
libdir      = ${exec_prefix}/lib
etcdir      = ${prefix}/etc

default_editor = /usr/bin/vi
default_pager  = /usr/bin/more

CFLAGS     = -Wall -Wno-pointer-sign -O2
INCLUDES   = -I.. -I$(top_srcdir) 
CONFIGDEFS = -DNMHBINDIR='"$(bindir)"' -DNMHETCDIR='"$(etcdir)"' -DNMHLIBDIR='"$(libdir)"' \
             -DDEFAULT_EDITOR='"$(default_editor)"' -DDEFAULT_PAGER='"$(default_pager)"'

COMPILE  = $(CC) -c $(DEFS) $(INCLUDES) $(CFLAGS)
COMPILE2 = $(CC) -c $(DEFS) $(CONFIGDEFS) $(INCLUDES) $(CFLAGS)

.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(COMPILE) $<

# source files
SRCS = config.c

# object files
OBJS = config.o version.o

# auxiliary files
AUX = Makefile.in version.sh

# all files in this directory included in the distribution
DIST = $(SRCS) $(AUX)

# ========= DEPENDENCIES FOR BUILDING ==========

all: $(OBJS)

version.c:
	${srcdir}/version.sh $(VERSION) > version.c

config.o: config.c
	$(COMPILE2) $(srcdir)/config.c

install:

uninstall:

lint:

# ========== DEPENDENCIES FOR CLEANUP ==========

mostlyclean:
	rm -f *.o *~

clean: mostlyclean
	rm -f version.c

distclean: clean
	#rm -f Makefile

realclean: distclean

superclean: realclean

# ========== DEPENDENCIES FOR MAINTENANCE ==========

subdir = config

distdir = ../`cat ../distname`/$(subdir)
nmhdist: $(DIST)
	@echo "Copying distribution files in $(subdir)"
	@for file in $(DIST); do \
	  cp -p $(srcdir)/$$file $(distdir); \
	done

