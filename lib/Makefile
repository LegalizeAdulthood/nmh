.POSIX:

SUBDIRS = lua mh

all clean install:
	@for d in $(SUBDIRS); do \
		(cd $$d && echo "===> $$d ($@)"; $(MAKE) $(MAKEFLAGS) $@) ;  \
	done

