.POSIX:

PREFIX = /usr/local

BINDIR = $(PREFIX)/bin
ETCDIR = $(PREFIX)/etc/nmh
LIBEXECDIR = $(PREFIX)/libexec/nmh

SUBDIRS = etc liblua libmh cmd libexec

PREREQ = h/paths.h

CLEANFILES = h/paths.h

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

h/paths.h: h/paths.h.in Makefile
	sed \
		-e "s;%BINDIR%;$(BINDIR);"		\
		-e "s;%ETCDIR%;$(ETCDIR);"		\
		-e "s;%LIBEXECDIR%;$(LIBEXECDIR);"	\
		h/paths.h.in > h/paths.h
