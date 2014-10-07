.POSIX:

CFLAGS = $(NMH_CFLAGS) -I../h -D_XOPEN_SOURCE=700
LDFLAGS = $(NHM_LDFLAGS) -lcurses -L../lib -lmh $(NMH_LDADD)

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

$(PROGS): ../h/mh.h ../h/_mhpaths.h ../h/mh_md5.h ../lib/libmh.a ../lib/liblua.a

clean:
	rm -f $(PROGS) $(PROGS:=.o)

install:

.PHONY: all clean install

.c:
	$(CC) -c $< $(CFLAGS)
	$(CC) -o $@ $*.o $(LDFLAGS)
