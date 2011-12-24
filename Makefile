.POSIX:
#
# Makefile for top level of nmh distribution
#

# nmh version
VERSION = posix-dev

SHELL = /bin/sh


srcdir = .
abs_srcdir = /Users/lyndon/src/nmh/posix


# ========== USER CONFIGURATION SECTION ==========
#
# If `make' is executed in the directory containing this Makefile,
# any changes made in this section will override the values of
# these parameters in makefiles in any of the subdirectories.

prefix      = /usr/local/nmh
exec_prefix = ${prefix}

# location of standard commands
bindir      = ${exec_prefix}/bin

# location of support binaries and scripts
libdir      = ${exec_prefix}/lib

# location of nmh configuration and formats files
etcdir      = ${prefix}/etc

# location of man pages
mandir      = ${prefix}/share/man

# RPM build root
rpmdir      = $(abs_srcdir)/RPM

# location of incoming mail
mailspool   = /var/mail

# location of mail transport agent
sendmailpath = /usr/sbin/sendmail

# default editor
default_editor = /usr/bin/vi

# default pager
default_pager = /usr/bin/more

CC       = gcc
CPPFLAGS = 
DEFS     = -DHAVE_CONFIG_H 
CFLAGS   = -Wall -Wno-pointer-sign -O2
LDFLAGS  = 
LIBS     = -liconv 

# ========== YOU SHOULDN'T HAVE TO CHANGE ANYTHING BELOW HERE ==========

# flags passed to recursive makes in subdirectories
MAKEDEFS = CC='$(CC)' CPPFLAGS='$(CPPFLAGS)' DEFS='$(DEFS)' \
CFLAGS='$(CFLAGS)' LDFLAGS='$(LDFLAGS)' LIBS='$(LIBS)' \
prefix='$(prefix)' exec_prefix='$(exec_prefix)' bindir='$(bindir)' \
etcdir='$(etcdir)' libdir='$(libdir)' mandir='$(mandir)' \
mailspool='$(mailspool)' sendmailpath='$(sendmailpath)' \
default_editor='$(default_editor)' default_pager='$(default_pager)'

INSTALL         = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA    = ${INSTALL} -m 644

.SUFFIXES:

# all files in this directory included in the distribution
DIST = ChangeLog COPYRIGHT DATE INSTALL MACHINES README VERSION		\
	ChangeLog install-sh mkinstalldirs

# subdirectories in distribution
SUBDIRS = h config sbr mts uip etc man docs

# ========== DEPENDENCIES FOR BUILDING AND INSTALLING ==========

# default target
all: config.h Makefile all-recursive

all-recursive:
	@for subdir in $(SUBDIRS); do \
	  echo "--> $$subdir"; \
	  (cd $$subdir && $(MAKE) $(MAKEDEFS) all) || exit 1; \
	done

install: all
	for subdir in $(SUBDIRS); do \
	  (cd $$subdir && $(MAKE) $(MAKEDEFS) $@) || exit 1; \
	done

uninstall:
	for subdir in $(SUBDIRS); do \
	  (cd $$subdir && $(MAKE) $(MAKEDEFS) $@) || exit 1; \
	done

# ========== DEPENDENCIES FOR CLEANUP ==========

mostlyclean: mostlyclean-recursive mostlyclean-local
clean:       clean-recursive       clean-local
distclean:   distclean-recursive   distclean-local
realclean:   realclean-recursive   realclean-local
superclean:  superclean-recursive  superclean-local

mostlyclean-local:
	rm -f *~

clean-local: mostlyclean-local

distclean-local: clean-local
	#rm -f Makefile config.h config.status config.log config.cache stamp-h distname
	$(RM) -r RPM

realclean-local: distclean-local

superclean-local: realclean-local
	cd $(srcdir) && rm -f config.h.in stamp-h.in configure

mostlyclean-recursive clean-recursive distclean-recursive realclean-recursive superclean-recursive:
	for subdir in $(SUBDIRS); do \
	  target=`echo $@ | sed 's/-recursive//'`; \
	  (cd $$subdir && $(MAKE) $(MAKEDEFS) $$target) || exit 1; \
	done

# ========== DEPENDENCIES FOR MAINTENANCE ==========

lint:
	for subdir in $(SUBDIRS); do \
	  ( cd $$subdir && $(MAKE) $(MAKEDEFS) lint ) || exit 1; \
	done

# ========== DEPENDENCIES FOR MAINTENANCE ==========


# name of new nmh distribution tar file
tarfile = nmh-$(VERSION).tar.gz

# ftp directory location
ftpdir = /ftp/nmh

# file containing name of new nmh distribution
distname:
	@echo nmh-$(VERSION) > distname

# build nmh distribution
distdir = `cat distname`
nmhdist: $(DIST) distname
	@echo "Begin building nmh-$(VERSION) distribution"
	rm -rf $(distdir)
	mkdir $(distdir)
	@chmod 755 $(distdir)
	@echo "Copying distribution files in main directory"
	@for file in $(DIST); do \
	  cp -p $(srcdir)/$$file $(distdir); \
	done
	@for subdir in $(SUBDIRS); do \
	  mkdir $(distdir)/$$subdir; \
	  chmod 755 $(distdir)/$$subdir; \
	  (cd $$subdir && $(MAKE) $@) || exit 1; \
	done
	chmod -R a+r $(distdir)
	tar chf - $(distdir) | gzip -c > $(tarfile)
	rm -rf $(distdir) distname
	@echo "Done building nmh-$(VERSION) distribution"

# release a new nmh distribution into ftp directory
nmhrelease:
	rm -f $(ftpdir)/$(tarfile)
	rm -f $(ftpdir)/nmh.tar.gz
	mv $(tarfile) $(ftpdir)/$(tarfile)
	cd $(ftpdir) && ln -s $(tarfile) nmh.tar.gz
	cd $(ftpdir) && md5sum *.gz > MD5SUM

# Create binary and source RPMs.  Assumes that configure had already
# been run.  Otherwise, there wouldn't be a Makefile.
# _topdir is the specified RPM build root.
# _tmppath is TMP below _topdir.
# srcdir is the top level nmh source directory.
# tarfile and version are as specified in this file.  rpmbuild does
#   not allow Version to contain any dashes.
rpm: all nmhdist
	@rpmbuild --define '_topdir $(rpmdir)' \
          --define '_tmppath %{_topdir}/TMP' \
          --define 'srcdir $(abs_srcdir)' \
          --define 'tarfile $(tarfile)' \
          --define 'version '`echo $(VERSION) | sed "s/-/_/g"` \
          -ba nmh.spec
	@$(RM) $(tarfile)
