.POSIX:

CFLAGS = $(NMH_CFLAGS) -I../h -D_XOPEN_SOURCE=600
LDFLAGS = $(NMH_LDFLAGS) -L../lib -lmh $(NMH_LDADD)

LUADEPS = ../h/lua.h ../lib/liblua.a
MHDEPS = ../h/mh.h ../h/_mhpaths.h ../lib/libmh.a

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
	whom


all: $(PROGS)

install:

clean:
	rm -f $(PROGS)

listprogs:
	@echo $(PROGS)

.PHONY: all clean install listprogs


ali: ali.c $(MHDEPS)
	$(CC) $(CFLAGS) -o ali ali.c $(LDFLAGS)

anno: anno.c $(MHDEPS)
	$(CC) $(CFLAGS) -o anno anno.c $(LDFLAGS)

burst: burst.c $(MHDEPS)
	$(CC) $(CFLAGS) -o burst burst.c $(LDFLAGS)

comp: comp.c $(MHDEPS)
	$(CC) $(CFLAGS) -o comp comp.c $(LDFLAGS) -lcurses

dist: dist.c $(MHDEPS)
	$(CC) $(CFLAGS) -o dist dist.c $(LDFLAGS) -lcurses

flist: flist.c $(MHDEPS)
	$(CC) $(CFLAGS) -o flist flist.c $(LDFLAGS)

fmttest: fmttest.c $(MHDEPS)
	$(CC) $(CFLAGS) -o fmttest fmttest.c $(LDFLAGS) -lcurses

folder: folder.c $(MHDEPS)
	$(CC) $(CFLAGS) -o folder folder.c $(LDFLAGS)

forw: forw.c $(MHDEPS)
	$(CC) $(CFLAGS) -o forw forw.c $(LDFLAGS) -lcurses

inc: inc.c $(MHDEPS)
	$(CC) $(CFLAGS) -o inc inc.c $(LDFLAGS) -lcurses

install-mh: install-mh.c $(MHDEPS)
	$(CC) $(CFLAGS) -o install-mh install-mh.c $(LDFLAGS)

mark: mark.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mark mark.c $(LDFLAGS)

mhbuild: mhbuild.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mhbuild mhbuild.c $(LDFLAGS)

mhfixmsg: mhfixmsg.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mhfixmsg mhfixmsg.c $(LDFLAGS) -lcurses

mhl: mhl.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mhl mhl.c $(LDFLAGS) -lcurses

mhlist: mhlist.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mhlist mhlist.c $(LDFLAGS) -lcurses

mhn: mhn.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mhn mhn.c $(LDFLAGS) -lcurses

mhparam: mhparam.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mhparam mhparam.c $(LDFLAGS)

mhpath: mhpath.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mhpath mhpath.c $(LDFLAGS)

mhshow: mhshow.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mhshow mhshow.c $(LDFLAGS) -lcurses

mhstore: mhstore.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mhstore mhstore.c $(LDFLAGS) -lcurses

msgchk: msgchk.c $(MHDEPS)
	$(CC) $(CFLAGS) -o msgchk msgchk.c $(LDFLAGS)

new: new.c $(MHDEPS)
	$(CC) $(CFLAGS) -o new new.c $(LDFLAGS)

packf: packf.c $(MHDEPS)
	$(CC) $(CFLAGS) -o packf packf.c $(LDFLAGS)

pick: pick.c $(MHDEPS)
	$(CC) $(CFLAGS) -o pick pick.c $(LDFLAGS)

refile: refile.c $(MHDEPS)
	$(CC) $(CFLAGS) -o refile refile.c $(LDFLAGS)

repl: repl.c $(MHDEPS)
	$(CC) $(CFLAGS) -o repl repl.c $(LDFLAGS) -lcurses

rmf: rmf.c $(MHDEPS)
	$(CC) $(CFLAGS) -o rmf rmf.c $(LDFLAGS)

rmm: rmm.c $(MHDEPS)
	$(CC) $(CFLAGS) -o rmm rmm.c $(LDFLAGS)

scan: scan.c $(MHDEPS)
	$(CC) $(CFLAGS) -o scan scan.c $(LDFLAGS) -lcurses

send: send.c $(MHDEPS)
	$(CC) $(CFLAGS) -o send send.c $(LDFLAGS)

show: show.c $(MHDEPS)
	$(CC) $(CFLAGS) -o show show.c $(LDFLAGS) -lcurses

sortm: sortm.c $(MHDEPS)
	$(CC) $(CFLAGS) -o sortm sortm.c $(LDFLAGS)

whatnow: whatnow.c $(MHDEPS)
	$(CC) $(CFLAGS) -o whatnow whatnow.c $(LDFLAGS)

whom: whom.c $(MHDEPS)
	$(CC) $(CFLAGS) -o whom whom.c $(LDFLAGS)

