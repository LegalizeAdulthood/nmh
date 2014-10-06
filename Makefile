.POSIX:

PREFIX = /usr/local

BINDIR = $(PREFIX)/bin
ETCDIR = $(PREFIX)/etc/nmh
LIBEXECDIR = $(PREFIX)/libexec/nmh

SUBDIRS = etc lib cmd libexec

INCLUDES = h/_mhpaths.h

CLEANFILES = $(INCLUDES)

.PHONY: all clean install prereq

all: prereq
	@for d in $(SUBDIRS); do	\
		(cd $$d && echo "===> $$d (all)"; $(MAKE) $(MAKEFLAGS)  all) ;	\
	done

clean install::
	@for d in $(SUBDIRS); do	\
		(cd $$d && echo "===> $$d ($@)"; $(MAKE) $(MAKEFLAGS)  $@) ;	\
	done

clean::
	rm -f $(CLEANFILES)

prereq: $(INCLUDES)

h/_mhpaths.h: h/_mhpaths.h.in Makefile
	sed \
		-e "s;%BINDIR%;$(BINDIR);"		\
		-e "s;%ETCDIR%;$(ETCDIR);"		\
		-e "s;%LIBEXECDIR%;$(LIBEXECDIR);"	\
		h/_mhpaths.h.in > h/_mhpaths.h
