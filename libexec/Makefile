.POSIX:

CFLAGS = $(NMH_CFLAGS) -I../h -D_XOPEN_SOURCE=700
LDFLAGS = $(NHM_LDFLAGS) -L../lib -lmh $(NMH_LDADD)

LUADEPS = ../h/lua.h ../lib/liblua.a
MHDEPS = ../h/mh.h ../h/_mhpaths.h ../lib/libmh.a


PROGS =	\
	ap		\
	dp		\
	fmtdump		\
	mkstemp		\
	post		\
	prompter	\
	rcvdist		\
	rcvpack		\
	rcvstore	\
	rcvtty		\
	slocal		\
	viamail


all: $(PROGS)

install:

clean:
	rm -f $(PROGS)

listprogs:
	@echo $(PROGS)

.PHONY: all clean install listprogs


ap: ap.c $(MHDEPS)
	$(CC) $(CFLAGS) -o ap ap.c $(LDFLAGS) -lcurses

dp: dp.c $(MHDEPS)
	$(CC) $(CFLAGS) -o dp dp.c $(LDFLAGS) -lcurses

fmtdump: fmtdump.c $(MHDEPS)
	$(CC) $(CFLAGS) -o fmtdump fmtdump.c $(LDFLAGS) -lcurses

mkstemp: mkstemp.c $(MHDEPS)
	$(CC) $(CFLAGS) -o mkstemp mkstemp.c $(LDFLAGS)

post: post.c $(MHDEPS)
	$(CC) $(CFLAGS) -o post post.c $(LDFLAGS)

prompter: prompter.c $(MHDEPS)
	$(CC) $(CFLAGS) -o prompter prompter.c $(LDFLAGS)

rcvdist: rcvdist.c $(MHDEPS)
	$(CC) $(CFLAGS) -o rcvdist rcvdist.c $(LDFLAGS) -lcurses

rcvpack: rcvpack.c $(MHDEPS)
	$(CC) $(CFLAGS) -o rcvpack rcvpack.c $(LDFLAGS)

rcvstore: rcvstore.c $(MHDEPS)
	$(CC) $(CFLAGS) -o rcvstore rcvstore.c $(LDFLAGS)

rcvtty: rcvtty.c $(MHDEPS)
	$(CC) $(CFLAGS) -o rcvtty rcvtty.c $(LDFLAGS) -lcurses

slocal: slocal.c $(MHDEPS)
	$(CC) $(CFLAGS) -o slocal slocal.c $(LDFLAGS)

viamail: viamail.c $(MHDEPS)
	$(CC) $(CFLAGS) -o viamail viamail.c $(LDFLAGS)

