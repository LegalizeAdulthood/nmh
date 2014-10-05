.POSIX:

PREFIX = /usr/local

BINDIR = $(PREFIX)/bin
ETCDIR = $(PREFIX)/etc/nmh
LIBEXECDIR = $(PREFIX)/libexec/nmh

SUBDIRS = etc liblua libmh cmd libexec

PREREQ = h/mhpaths.h

CLEANFILES = h/mhpaths.h

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

prereq: $(PREREQ)

h/mhpaths.h: h/mhpaths.h.in Makefile
	sed \
		-e "s;%BINDIR%;$(BINDIR);"		\
		-e "s;%ETCDIR%;$(ETCDIR);"		\
		-e "s;%LIBEXECDIR%;$(LIBEXECDIR);"	\
		h/mhpaths.h.in > h/mhpaths.h
