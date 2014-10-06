.POSIX:

CFLAGS = -I.. -I../h -D_XOPEN_SOURCE=700 -Werror
LDFLAGS = -lcurses -L../libmh -lmh

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

clean:
	rm -f $(PROGS) $(PROGS:=.o)

install:

.PHONY: all clean install

.c:
	$(CC) -c $< $(CFLAGS)
	$(CC) -o $@ $*.o $(LDFLAGS)
