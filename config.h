/****** BEGIN USER CONFIGURATION SECTION *****/

/*
 * IMPORTANT: You should no longer need to edit this file to handle
 * your operating system. That should be handled and set correctly by
 * configure now.
 *
 * These are slowly being phased out, but currently
 * not everyone is auto-configured.  Then decide if you
 * wish to change the features that are compiled into nmh.
 */

/*
 * If you have defined DOT_LOCKING, then the default is to
 * place the lock files in the same directory as the file that
 * is to be locked.  Alternately, if you define LOCKDIR, you
 * can specify that all lock files go in a specific directory.
 * Don't define this unless you know you need it.
 */
/* #define LOCKDIR "/usr/spool/locks" */

/*
 * Define this if your passwords are stored in some type of
 * distributed name service, such as NIS, or NIS+.
 */
#define DBMPWD  1

/*
 * Directs nmh not to try and rewrite addresses
 * to their official form.  You probably don't
 * want to change this without good reason.
 */
#define DUMB    1

/*
 * Define this if you do not want nmh to attach the local hostname
 * to local addresses.  You must also define DUMB.  You probably
 * don't need this unless you are behind a firewall.
 */
/* #define REALLYDUMB  1 */

/*
 * Starting on January 1, 2000, some MUAs like ELM and Ultrix's DXmail started
 * generated bad dates ("00" or "100" for the year).  If this #define is active,
 * we use windowing to correct those dates to what we presume to be the intended
 * values.  About the only time this could get us into trouble would be if a MUA
 * was generating a year of "00" in 2001 or later, due to an unrelated bug.  In
 * this case we would "correct" the year to 2000, which could result in
 * inaccurate bug reports against the offending MUA.  A much more esoteric case
 * in which you might not want to #define this would be if you were OCR'ing in
 * old written correspondence and saving it in email format, and you had dates
 * of 1899 or earlier.
 */
#define FIX_NON_Y2K_COMPLIANT_MUA_DATES 1

/*
 * Directs inc/slocal to extract the envelope sender from "From "
 * line.  If inc/slocal is saving message to folder, then this
 * sender information is then used to create a Return-Path
 * header which is then added to the message.
 */
#define RPATHS  1

/*
 * If defined, slocal will use `mbox' format when saving to
 * your standard mail spool.  If not defined, it will use
 * mmdf format.
 */
#define SLOCAL_MBOX  1

/*
 * If this is defined, nmh will recognize the ~ construct.
 */
#define MHRC    1

/*
 * Compile simple ftp client into mhn.  This will be used by mhn
 * for ftp access unless you have specified another access method
 * in your .mh_profile or mhn.defaults.  Use the "mhn-access-ftp"
 * profile entry to override this.  Check mhn(1) man page for
 * details.
 */
#define BUILTIN_FTP 1

/*
 * If you enable POP support, this is the the port name that nmh will use.  Make
 * sure this is defined in your /etc/services file (or its NIS/NIS+ equivalent).
 * If you are using KPOP, you will need to change this to "kpop" unless you want
 * to be able to use both POP3 _and_ Kerberized POP and plan to use inc and
 * msgchk's -kpop switch every time in the latter case.
 */
#define POPSERVICE "pop3"

/*
 * Define the default creation modes for folders and messages.
 */
#define DEFAULT_FOLDER_MODE "700"
#define DEFAULT_MESSAGE_MODE "600"

/*
 * Name of link to file to which you are replying.
 */
#define LINK "@"

/*
 * Define to 1 if your vi has ATT bug, such that it returns
 * non-zero exit codes on `pseudo-errors'.
 */
/* #undef ATTVIBUG */


/***** END USER CONFIGURATION SECTION *****/

/* The prefix that is prepended to the name of message files when they are
   "removed" by rmm. This should typically be `,' or `#'. */
#define BACKUP_PREFIX ","

/* Defined for SunOS 4, FreeBSD, NetBSD, OpenBSD, BSD/OS, Mac OS X/Rhapsody --
   does PicoBSD have uname? */
/* #undef BSD42 */

/* Defined for SunOS 4, FreeBSD, NetBSD, OpenBSD, BSD/OS, Mac OS X/Rhapsody;
   only used in vmh.c which is not built. */
/* #undef BSD44 */

/* Define to 1 if you use BSD style signal handling (and can block signals).
   */
/* #undef BSD_SIGNALS */

/* Define to use the Cyrus SASL library for authentication of POP and SMTP. */
/* #undef CYRUS_SASL */

/* Define to use dot based file locking. */
#define DOT_LOCKING 1

/* Define to use fnctl() based locking. */
/* #undef FCNTL_LOCKING */

/* Define to use flock() based locking. */
/* #undef FLOCK_LOCKING */

/* Define to 1 if `TIOCGWINSZ' requires <sys/ioctl.h>. */
/* #undef GWINSZ_IN_SYS_IOCTL */

/* Define to 1 if you have the <crypt.h> header file. */
/* #undef HAVE_CRYPT_H */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `getutent' function. */
/* #undef HAVE_GETUTENT */

/* Define if you have the iconv() function. */
#define HAVE_ICONV 1

/* Define to 1 if you have the <iconv.h> header file. */
#define HAVE_ICONV_H 1

/* Define to 1 if you have the <langinfo.h> header file. */
#define HAVE_LANGINFO_H 1

/* Define to 1 if you have the `crypto' library (-lcrypto). */
/* #undef HAVE_LIBCRYPTO */

/* Define to 1 if you have the `lockfile' library (-llockfile). */
/* #undef HAVE_LIBLOCKFILE */

/* Define to 1 if you have the `sasl2' library (-lsasl2). */
/* #undef HAVE_LIBSASL2 */

/* Define to 1 if you have the `ssl' library (-lssl). */
/* #undef HAVE_LIBSSL */

/* Define to 1 if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define to 1 if you have the `mbtowc' function. */
#define HAVE_MBTOWC 1

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the `nl_langinfo' function. */
#define HAVE_NL_LANGINFO 1

/* Define to 1 if you have the `sigprocmask' function. */
#define HAVE_SIGPROCMASK 1

/* Define if there is a member named d_type in the struct describing directory
   headers. */
#define HAVE_STRUCT_DIRENT_D_TYPE 1

/* Define to 1 if `st_blksize' is member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_BLKSIZE 1

/* Define to 1 if `tm_gmtoff' is member of `struct tm'. */
#define HAVE_STRUCT_TM_TM_GMTOFF 1

/* Define to 1 if `ut_type' is member of `struct utmp'. */
/* #undef HAVE_STRUCT_UTMP_UT_TYPE */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/utsname.h> header file. */
#define HAVE_SYS_UTSNAME_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <termcap.h> header file. */
#define HAVE_TERMCAP_H 1

/* Define to 1 if you have the `tzset' function. */
#define HAVE_TZSET 1

/* Define to 1 if you have the \`union wait' type in <sys/wait.h>. */
/* #undef HAVE_UNION_WAIT */

/* Define to 1 if you have the `vfork' function. */
#define HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the `wait3' function. */
#define HAVE_WAIT3 1

/* Define to 1 if you have the `waitpid' function. */
#define HAVE_WAITPID 1

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define to 1 if you have the <wctype.h> header file. */
#define HAVE_WCTYPE_H 1

/* Define to 1 if you have the `wcwidth' function. */
#define HAVE_WCWIDTH 1

/* Define to 1 if you have the `writev' function. */
#define HAVE_WRITEV 1

/* Define as const if the declaration of iconv() needs const. */
#define ICONV_CONST 

/* Define to the header containing the declaration of `initgroups'. */
#define INITGROUPS_HEADER <unistd.h>

/* Use the Linux _IO_*_ptr defines from <libio.h>. */
/* #undef LINUX_STDIO */

/* Undefine if you don't want locale features. By default this is defined. */
#define LOCALE 1

/* Define to use lockf() based locking. */
/* #undef LOCKF_LOCKING */

/* Define to 1 if you need to make `inc' set-group-id because your mail spool
   is not world writable. There are no guarantees as to the safety of doing
   this, but this #define will add some extra security checks. */
#define MAILGROUP 1

/* Define to compile in support for the Emacs front-end mh-e. */
#define MHE 1

/* Define to enable support for multibyte character sets. */
#define MULTIBYTE_SUPPORT 1

/* Define to 1 if you have ospeed, but it is not defined in termcap.h. */
/* #undef MUST_DEFINE_OSPEED */

/* Define to the header containing the ndbm API prototypes. */
#define NDBM_HEADER <ndbm.h>

/* Define to 1 if you have no signal blocking at all (bummer). */
/* #undef NO_SIGNAL_BLOCKING */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "nmh"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "nmh popsix-dev"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "nmh"

/* Define to the version of this package. */
#define PACKAGE_VERSION "posix-dev"

/* Define this to compile client-side support for pop into inc and msgchk. */
/* #undef POP */

/* Define to 1 if you use POSIX style signal handling. */
#define POSIX_SIGNALS 1

/* Define to 1 if you have reliable signals. */
#define RELIABLE_SIGNALS 1

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Defined for SCO5. */
/* #undef SCO_5_STDIO */

/* Define if you want SMTP (simple mail transport protocol) support. */
#define SMTPMTS 1

/* Define to 1 if the `S_IS*' macros in <sys/stat.h> do not work properly. */
/* #undef STAT_MACROS_BROKEN */

/* Defined for Solaris 2.x, Irix, OSF/1, HP-UX, AIX; only used in vmh.c which
   is not built. */
/* #undef SVR4 */

/* Defined for Solaris 2.x, Irix, OSF/1, HP-UX, AIX, SCO5; only used in vmh.c
   which is not built. */
/* #undef SYS5 */

/* Define to 1 if you use SYSV style signal handling (and can block signals).
   */
/* #undef SYSV_SIGNALS */

/* Define to 1 if tgetent() accepts NULL as a buffer. */
#define TGETENT_ACCEPTS_NULL 1

/* Define to what tgetent() returns on success (0 on HP-UX X/Open curses). */
#define TGETENT_SUCCESS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Support TLS for session encryption. */
/* #undef TLS_SUPPORT */

/* Define to 1 if `struct winsize' requires <sys/ptem.h>. */
/* #undef WINSIZE_IN_PTEM */

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1
