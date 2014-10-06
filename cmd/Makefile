.POSIX:

CFLAGS = -I.. -I../h -D_XOPEN_SOURCE=700 -Werror
LDFLAGS = -lcurses -L../libmh -lmh

PROGS =		\
	ali	\
	anno	\
	burst	\
	comp	\
	dist	\
	flist	\
	fmttest	\
	folder	\
	forw	\
	inc	\
	install-mh	\
	mark	\
	mhbuild	\
	mhfixmsg	\
	mhl	\
	mhlist	\
	mhn	\
	mhparam	\
	mhpath	\
	mhshow	\
	mhstore	\
	msgchk	\
	new	\
	packf	\
	pick	\
	refile	\
	repl	\
	rmf	\
	rmm	\
	scan	\
	send	\
	show	\
	sortm	\
	whatnow	\
	whom	\


all: $(PROGS)

clean:

install:

.PHONY: all clean install

.c:
	$(CC) -c $< $(CFLAGS)
	$(CC) -o $@ $*.o $(LDFLAGS)
