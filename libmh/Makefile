.POSIX:

LIB = libmh.a
CFLAGS = -I../h -I.. -D_XOPEN_SOURCE=700 -Werror

CLEANFILES = dtimep.c

SRCS = \
	addrsbr.c	\
	aliasbr.c	\
	ambigsw.c	\
	annosbr.c	\
	arglist.c	\
	atooi.c		\
	base64.c	\
	brkstring.c	\
	charstring.c	\
	check_charset.c	\
	client.c	\
	closefds.c	\
	concat.c	\
	config.c	\
	context_del.c	\
	context_find.c	\
	context_foil.c	\
	context_read.c	\
	context_replace.c	\
	context_save.c	\
	copy.c		\
	copyip.c	\
	cpydata.c	\
	cpydgst.c	\
	crawl_folders.c	\
	credentials.c	\
	discard.c	\
	distsbr.c	\
	done.c		\
	dropsbr.c	\
	dtime.c		\
	dtimep.c	\
	encode_rfc2047.c	\
	error.c	\
	escape_addresses.c	\
	ext_hook.c	\
	fdcompare.c	\
	fmt_addr.c	\
	fmt_compile.c	\
	fmt_new.c	\
	fmt_rfc2047.c	\
	fmt_scan.c	\
	folder_addmsg.c	\
	folder_delmsgs.c	\
	folder_free.c	\
	folder_pack.c	\
	folder_read.c	\
	folder_realloc.c	\
	forwsbr.c	\
	gans.c	\
	getans.c	\
	getansreadline.c	\
	getanswer.c	\
	getarguments.c	\
	getcpy.c	\
	geteditor.c	\
	getfolder.c	\
	getpass.c	\
	lock_file.c	\
	m_atoi.c	\
	m_backup.c	\
	m_convert.c	\
	m_draft.c	\
	m_getfld.c	\
	m_gmprot.c	\
	m_maildir.c	\
	m_mktemp.c	\
	m_name.c	\
	m_popen.c	\
	m_rand.c	\
	makedir.c	\
	message_id.c	\
	mf.c	\
	mh_md5.c	\
	mhbuildsbr.c	\
	mhcachesbr.c	\
	mhfree.c	\
	mhlistsbr.c	\
	mhlsbr.c	\
	mhmisc.c	\
	mhoutsbr.c	\
	mhparse.c	\
	mhshowsbr.c	\
	mhstoresbr.c	\
	mime_type.c	\
	mts.c	\
	norm_charmap.c	\
	path.c	\
	peekc.c	\
	picksbr.c	\
	pidstatus.c	\
	pidwait.c	\
	popsbr.c	\
	print_help.c	\
	print_sw.c	\
	print_version.c	\
	push.c	\
	putenv.c	\
	r1bindex.c	\
	readconfig.c	\
	refile.c	\
	remdir.c	\
	replsbr.c	\
	ruserpass.c	\
	scansbr.c	\
	sendsbr.c	\
	seq_add.c	\
	seq_bits.c	\
	seq_del.c	\
	seq_getnum.c	\
	seq_list.c	\
	seq_nameok.c	\
	seq_print.c	\
	seq_read.c	\
	seq_save.c	\
	seq_setcur.c	\
	seq_setprev.c	\
	seq_setunseen.c	\
	showfile.c	\
	signals.c	\
	smatch.c	\
	smtp.c		\
	snprintb.c	\
	ssequal.c	\
	strindex.c	\
	terminal.c	\
	trimcpy.c	\
	unquote.c	\
	uprf.c	\
	utils.c	\
	vector.c	\
	version.c	\
	vfgets.c	\
	whatnowproc.c	\
	whatnowsbr.c

OBJS = $(SRCS:.c=.o)


all: $(LIB)

$(LIB): $(OBJS)
	$(AR) -cr $(LIB) `lorder $(OBJS) | tsort -q`

tsort:
	lorder $(OBJS) | tsort -q

clean:
	rm -f $(OBJS) $(LIB) $(CLEANFILES)

install:

.PHONY: all clean install tsort
