/*
 * mh.h -- main header file for all of nmh
 */

/*
 * XXX
 *
 * When compiling with _XOPEN_SOURCE=700, Solaris 11 does not bring
 * str[n]casenmp() and snprintf() into scope. This is a bug in their
 * implementation that, unfortunately, requires dropping this anvil on
 * their head.
 */
#ifdef __SunOS_5_11
# define __EXTENSIONS__
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <_mhpaths.h>

/*
 * The following can be overriden in the Makefile.
 */
#ifndef BACKUP_PREFIX
# define BACKUP_PREFIX ","
#endif
#ifndef MAILSPOOL
# define MAILSPOOL "/var/spool/mail"
#endif
#ifndef SENDMAILPATH
# define SENDMAILPATH "/usr/sbin/sendmail"
#endif


#define DEFAULT_LOCKING "fcntl" /* XXX clean up lock_file.c to get rid of this */

/*
 * Well-used constants
 */
#define	NOTOK        (-1)	/* syscall()s return this on error */
#define	OK             0	/*  ditto on success               */
#define	DONE           1	/* trinary logic                   */
#define ALL           ""
#define	Nbby           8	/* number of bits/byte */

#define MAXARGS	    1000	/* max arguments to exec                */
#define NFOLDERS    1000	/* max folder arguments on command line */
#define DMAXFOLDER     4	/* typical number of digits             */
#define MAXFOLDER   1000	/* message increment                    */

/*
 * This macro is for use by scan, for example, so that platforms with
 * a small BUFSIZ can easily allocate larger buffers.
 */
#define NMH_BUFSIZ  (BUFSIZ>=8192 ? BUFSIZ : 8192)

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
typedef unsigned char  boolean;  /* not int so we can pack in a structure */

/* If we're using gcc then give it some information about
 * functions that abort.
 */
#if __GNUC__ > 2
#define NORETURN __attribute__((__noreturn__))
#define NMH_UNUSED(i) (void) i
#else
#define NORETURN
#define NMH_UNUSED(i) i
#endif


/*
 * The type for a signal handler
 */
typedef void (*SIGNAL_HANDLER)(int);

/*
 * prototypes
 */
SIGNAL_HANDLER SIGNAL (int, SIGNAL_HANDLER);
SIGNAL_HANDLER SIGNAL2 (int, SIGNAL_HANDLER);


/*
 * char array that keeps track of size in both bytes and characters
 * Usage note:
 *    Don't store return value of charstring_buffer() and use later
 *    after intervening push_back's; use charstring_buffer_copy()
 *    instead.
 */
typedef struct charstring *charstring_t;

charstring_t charstring_create (size_t);
charstring_t charstring_copy (const charstring_t);
void charstring_free (charstring_t);
/* Append a single-byte character: */
void charstring_push_back (charstring_t, const char);
/* Append possibly multi-byte character(s): */
void charstring_push_back_chars (charstring_t, const char [], size_t, size_t);
void charstring_append (charstring_t, const charstring_t);
void charstring_clear (charstring_t);
/* Don't store return value of charstring_buffer() and use later after
   intervening push_back's; use charstring_buffer_copy() instead. */
const char *charstring_buffer (const charstring_t);
/* User is responsible for free'ing result of buffer copy. */
char *charstring_buffer_copy (const charstring_t);
size_t charstring_bytes (const charstring_t);
size_t charstring_chars (const charstring_t);
/* Length of the last character in the charstring. */
int charstring_last_char_len (const charstring_t);

/*
 * user context/profile structure
 */
struct node {
    char *n_name;		/* key                  */
    char *n_field;		/* value                */
    char  n_context;		/* context, not profile */
    struct node *n_next;	/* next entry           */
};

/*
 * switches structure
 */
#define	AMBIGSW	 (-2)	/* from smatch() on ambiguous switch */
#define	UNKWNSW	 (-1)	/* from smatch() on unknown switch   */

struct swit {

    /*
     * Switch name
     */

    char *sw;

    /* The minchars field is apparently used like this:

       -# : Switch can be abbreviated to # characters; switch hidden in -help.
       0  : Switch can't be abbreviated;               switch shown in -help.
       #  : Switch can be abbreviated to # characters; switch shown in -help. */
    int minchars;

    /*
     * If we pick this switch, return this value from smatch
     */

    int swret;
};

/*
 * Macros to use when declaring struct swit arrays.
 *
 * These macros use a technique known as X-Macros.  In your source code you
 * use them like this:
 *
 * #define FOO_SWITCHES \
 *    X("switch1", 0, SWITCHSW) \
 *    X("switch2", 0, SWITCH2SW) \
 *    X("thirdswitch", 2, SWITCH3SW) \
 *
 * The argument to each entry in FOO_SWITCHES are the switch name (sw),
 * the minchars field (see above) and the return value for this switch.
 * Note that the last entry in the above definition must either omit the
 * continuation backslash, or be followed by a blank line.  In the nmh
 * code the style is to have every line include a backslash and follow
 * the SWITCHES macro definition by a blank line.
 *
 * After you define FOO_SWITCHES, you instantiate it as follows:
 *
 * #define X(sw, minchars, id) id,
 * DEFINE_SWITCH_ENUM(FOO);
 * #undef X
 *
 * #define X(sw, minchars, id) { sw, minchars, id },
 * DEFINE_SWITCH_ARRAY(FOO);
 * #undef X
 *
 * DEFINE_SWITCH_ENUM defines an extra enum at the end of the list called
 * LEN_FOO.
 */

#define DEFINE_SWITCH_ENUM(name) \
    enum { \
    	name ## _SWITCHES \
	LEN_ ## name \
    }

#define DEFINE_SWITCH_ARRAY(name, array) \
    static struct swit array[] = { \
    	name ## _SWITCHES \
	{ NULL, 0, 0 } \
    }

extern struct swit anoyes[];	/* standard yes/no switches */

/*
 * general folder attributes
 */
#define READONLY   (1<<0)	/* No write access to folder    */
#define	SEQMOD	   (1<<1)	/* folder's sequences modifed   */
#define	ALLOW_NEW  (1<<2)	/* allow the "new" sequence     */
#define	OTHERS	   (1<<3)	/* folder has other files	*/
#define	MODIFIED   (1<<4)	/* msh in-core folder modified  */

#define	FBITS "\020\01READONLY\02SEQMOD\03ALLOW_NEW\04OTHERS\05MODIFIED"

/*
 * first free slot for user defined sequences
 * and attributes
 */
#define	FFATTRSLOT  5

/*
 * internal messages attributes (sequences)
 */
#define EXISTS        (0)	/* exists            */
#define DELETED       (1)	/* deleted           */
#define SELECTED      (2)	/* selected for use  */
#define SELECT_EMPTY  (3)	/* "new" message     */
#define	SELECT_UNSEEN (4)	/* inc/show "unseen" */

#define	MBITS "\020\01EXISTS\02DELETED\03SELECTED\04NEW\05UNSEEN"

/*
 * type for holding the sequence set of a message
 */
typedef struct bvector *bvector_t;

bvector_t bvector_create (size_t /* initial size in bits, can be 0 */);
void bvector_copy (bvector_t, bvector_t);
void bvector_free (bvector_t);
void bvector_clear (bvector_t, size_t);
void bvector_clear_all (bvector_t);
void bvector_set (bvector_t, size_t);
unsigned int bvector_at (bvector_t, size_t);
const unsigned long *bvector_bits (bvector_t);
size_t bvector_maxsize (bvector_t);

typedef struct svector *svector_t;

svector_t svector_create (size_t);
void svector_free (svector_t);
char *svector_push_back (svector_t, char *);
char *svector_at (svector_t, size_t);
char **svector_find(svector_t, const char *);
char **svector_strs (svector_t);
size_t svector_size (svector_t);

typedef struct ivector *ivector_t;

ivector_t ivector_create (size_t);
void ivector_free (ivector_t);
int ivector_push_back (ivector_t, int);
int ivector_at (ivector_t, size_t);
int *ivector_atp (ivector_t, size_t);
size_t ivector_size (ivector_t);

/*
 * Primary structure of folder/message information
 */
struct msgs {
    int lowmsg;		/* Lowest msg number                 */
    int hghmsg;		/* Highest msg number                */
    int nummsg;		/* Actual Number of msgs             */

    int lowsel;		/* Lowest selected msg number        */
    int hghsel;		/* Highest selected msg number       */
    int numsel;		/* Number of msgs selected           */

    int curmsg;		/* Number of current msg if any      */

    int msgflags;	/* Folder attributes (READONLY, etc) */
    char *foldpath;	/* Pathname of folder                */

    /*
     * Name of sequences in this folder.
     */
    svector_t msgattrs;

    /*
     * bit flags for whether sequence
     * is public (0), or private (1)
     */
    bvector_t attrstats;

    /*
     * These represent the lowest and highest possible
     * message numbers we can put in the message status
     * area, without calling folder_realloc().
     */
    int	lowoff;
    int	hghoff;

    /*
     * This is an array of bvector_t which we allocate dynamically.
     * Each bvector_t is a set of bits flags for a particular message.
     * These bit flags represent general attributes such as
     * EXISTS, SELECTED, etc. as well as track if message is
     * in a particular sequence.
     */
    size_t num_msgstats;
    bvector_t *msgstats;	/* msg status */

    /*
     * A FILE handle containing an open filehandle for the sequence file
     * for this folder.  If non-NULL, use it when the sequence file is
     * written.
     */
    FILE *seqhandle;

    /*
     * The name of the public sequence file; required by lkfclose()
     */
    char *seqname;
};

/*
 * Amount of space to allocate for msgstats.  Allocate
 * the array to have space for messages numbered lo to hi.
 * Use MSGSTATNUM to load mp->num_msgstats first.
 */
#define MSGSTATNUM(lo, hi) ((size_t) ((hi) - (lo) + 1))
#define MSGSTATSIZE(mp) ((mp)->num_msgstats * sizeof *(mp)->msgstats)

/*
 * macros for message and sequence manipulation
 */
#define msgstat(mp,n) (mp)->msgstats[(n) - mp->lowoff]
#define clear_msg_flags(mp,msgnum)   bvector_clear_all (msgstat(mp, msgnum))
#define copy_msg_flags(mp,i,j)       bvector_copy (msgstat(mp,i), msgstat(mp,j))
#define get_msg_flags(mp,ptr,msgnum) bvector_copy (ptr, msgstat(mp, msgnum))
#define set_msg_flags(mp,ptr,msgnum) bvector_copy (msgstat(mp, msgnum), ptr)

#define does_exist(mp,msgnum)     bvector_at (msgstat(mp, msgnum), EXISTS)
#define unset_exists(mp,msgnum)   bvector_clear (msgstat(mp, msgnum), EXISTS)
#define set_exists(mp,msgnum)     bvector_set (msgstat(mp, msgnum), EXISTS)

#define is_selected(mp,msgnum)    bvector_at (msgstat(mp, msgnum), SELECTED)
#define unset_selected(mp,msgnum) bvector_clear (msgstat(mp, msgnum), SELECTED)
#define set_selected(mp,msgnum)   bvector_set (msgstat(mp, msgnum), SELECTED)

#define is_select_empty(mp,msgnum)  \
        bvector_at (msgstat(mp, msgnum), SELECT_EMPTY)
#define set_select_empty(mp,msgnum) \
        bvector_set (msgstat(mp, msgnum), SELECT_EMPTY)

#define is_unseen(mp,msgnum) \
        bvector_at (msgstat(mp, msgnum), SELECT_UNSEEN)
#define unset_unseen(mp,msgnum) \
        bvector_clear (msgstat(mp, msgnum), SELECT_UNSEEN)
#define set_unseen(mp,msgnum) \
        bvector_set (msgstat(mp, msgnum), SELECT_UNSEEN)

/* for msh only */
#define set_deleted(mp,msgnum)     bvector_set (msgstat(mp, msgnum), DELETED)

#define in_sequence(mp,seqnum,msgnum) \
        bvector_at (msgstat(mp, msgnum), FFATTRSLOT + seqnum)
#define clear_sequence(mp,seqnum,msgnum) \
        bvector_clear (msgstat(mp, msgnum), FFATTRSLOT + seqnum)
#define add_sequence(mp,seqnum,msgnum) \
        bvector_set (msgstat(mp, msgnum), FFATTRSLOT + seqnum)

#define is_seq_private(mp,seqnum) \
        bvector_at (mp->attrstats, FFATTRSLOT + seqnum)
#define make_seq_public(mp,seqnum) \
        bvector_clear (mp->attrstats, FFATTRSLOT + seqnum)
#define make_seq_private(mp,seqnum) \
        bvector_set (mp->attrstats, FFATTRSLOT + seqnum)
#define make_all_public(mp) \
        mp->attrstats = bvector_create(0); bvector_clear_all (mp->attrstats)

/*
 * macros for folder attributes
 */
#define clear_folder_flags(mp) ((mp)->msgflags = 0)

#define is_readonly(mp)     ((mp)->msgflags & READONLY)
#define set_readonly(mp)    ((mp)->msgflags |= READONLY)

#define other_files(mp)     ((mp)->msgflags & OTHERS)
#define set_other_files(mp) ((mp)->msgflags |= OTHERS)

#define	NULLMP	((struct msgs *) 0)

/*
 * m_getfld() message parsing
 */

#define NAMESZ  999		/* Limit on component name size.
				   RFC 2822 limits line lengths to
				   998 characters, so a header name
				   can be at most that long.
				   m_getfld limits header names to 2
				   less than NAMESZ, which is fine,
				   because header names must be
				   followed by a colon.	 Add one for
				   terminating NULL. */

#define LENERR  (-2)		/* Name too long error from getfld  */
#define FMTERR  (-3)		/* Message Format error             */
#define FLD      0		/* Field returned                   */
#define FLDPLUS  1		/* Field returned with more to come */
#define BODY     3		/* Body  returned with more to come */
#define FILEEOF  5		/* Reached end of input file        */

struct m_getfld_state;
typedef struct m_getfld_state *m_getfld_state_t;

/*
 * Maildrop styles
 */
#define	MS_DEFAULT	0	/* default (one msg per file) */
#define	MS_UNKNOWN	1	/* type not known yet         */
#define	MS_MBOX		2	/* Unix-style "from" lines    */
#define	MS_MMDF		3	/* string mmdlm2              */
#define	MS_MSH		4	/* whacko msh                 */

#define	NOUSE	0		/* draft being re-used */

#define TFOLDER 0		/* path() given a +folder */
#define TFILE   1		/* path() given a file    */
#define	TSUBCWF	2		/* path() given a @folder */

#define OUTPUTLINELEN	72	/* default line length for headers */

#define LINK	"@"		/* Name of link to file to which you are */
				/* replying. */

/*
 * credentials management
 */
struct nmh_creds {
    char *host;
    char *user;
    char *password;
};

typedef struct nmh_creds *nmh_creds_t;

/*
 * miscellaneous macros
 */
#define	pidXwait(pid,cp) pidstatus (pidwait (pid, NOTOK), stdout, cp)

#ifndef max
# define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
# define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef abs
# define abs(a) ((a) > 0 ? (a) : -(a))
#endif

/*
 * GLOBAL VARIABLES
 */
#define CTXMOD	0x01		/* context information modified */
#define	DBITS	"\020\01CTXMOD"
extern char ctxflags;

extern char *invo_name;		/* command invocation name         */
extern char *mypath;		/* user's $HOME                    */
extern char *defpath;		/* pathname of user's profile      */
extern char *ctxpath;		/* pathname of user's context      */
extern struct node *m_defs;	/* list of profile/context entries */

/* What style to use for generated Message-ID and Content-ID header
   fields.  The localname style is pid.time@localname, where time is
   in seconds.  The random style replaces the localname with some
   (pseudo)random bytes and uses microsecond-resolution time. */
int save_message_id_style (const char *);
char *message_id (time_t, int);

/*
 * These standard strings are defined in config.c.  They are the
 * only system-dependent parameters in nmh, and thus by redefining
 * their values and reloading the various modules, nmh will run
 * on any system.
 */
extern char *buildmimeproc;
extern char *catproc;
extern char *components;
extern char *context;
extern char *current;
extern char *credentials_file;
extern char *defaultfolder;
extern char *digestcomps;
extern char *distcomps;
extern char *draft;
extern char *fileproc;
extern char *foldprot;
extern char *formatproc;
extern char *forwcomps;
extern char *inbox;
extern char *incproc;
extern char *lproc;
extern char *mailproc;
extern char *mh_defaults;
extern char *mh_profile;
extern char *mh_seq;
extern char *mhlformat;
extern char *mhlforward;
extern char *mhlproc;
extern char *mhlreply;
extern char *moreproc;
extern char *msgprot;
extern char *nmhaccessftp;
extern char *nmhaccessurl;
extern char *nmhstorage;
extern char *nmhcache;
extern char *nmhprivcache;
extern char *nsequence;
extern char *packproc;
extern char *postproc;
extern char *pfolder;
extern char *psequence;
extern char *rcvdistcomps;
extern char *rcvstoreproc;
extern char *replcomps;
extern char *replgroupcomps;
extern char *rmmproc;
extern char *sendproc;
extern char *showmimeproc;
extern char *showproc;
extern char *usequence;
extern char *version_num;
extern char *version_str;
extern char *whatnowproc;
extern char *whomproc;

extern void (*done) (int) NORETURN;

/* m_mktemp.c */
char *m_mktemp(const char *, int *, FILE **);
char *m_mktemp2(const char *, const char *, int *, FILE **);
char *m_mktemps(const char *pfx, const char *suffix, int *, FILE **);
char *get_temp_dir();


/*
 * prototypes.h -- various prototypes
 *
 * If you modify functions here, please document their current behavior
 * as much as practical.
 */

/*
 * prototype from config.h
 */
char *etcpath(char *);

/*
 * prototypes from the nmh subroutine library
 */

struct msgs_array;

void add_profile_entry (const char *, const char *);
void adios (char *, char *, ...) NORETURN;
void admonish (char *, char *, ...);
void advertise (char *, char *, char *, va_list);
void advise (char *, char *, ...);
char **argsplit (char *, char **, int *);
void argsplit_msgarg (struct msgs_array *, char *, char **);
void argsplit_insert (struct msgs_array *, char *, char **);
void arglist_free (char *, char **);
void ambigsw (char *, struct swit *);
int atooi(char *);
char **brkstring (char *, char *, char *);

/*
 * Check to see if we can display a given character set natively.
 * Arguments include:
 *
 * str	- Name of character set to check against
 * len	- Length of "str"
 *
 * Returns 1 if the specified character set can be displayed natively,
 * 0 otherwise.
 */

int check_charset (char *, int);
int client(char *, char *, char *, int, int);
void closefds(int);
char *concat (const char *, ...);
int context_del (char *);
char *context_find (const char *);
char *context_find_by_type (const char *, const char *, const char *);
int context_foil (char *);
void context_read (void);
void context_replace (char *, char *);
void context_save (void);
char *copy (const char *, char *);
char **copyip (char **, char **, int);
void cpydata (int, int, char *, char *);
void cpydgst (int, int, char *, char *);
char *cpytrim (const char *);
char *rtrim (char *);
int decode_rfc2047 (char *, char *, size_t);
void discard (FILE *);
char *upcase (const char *);

/*
 * Decode two characters into their quoted-printable representation.
 *
 * Arguments are:
 *
 * byte1	- First character of Q-P representation
 * byte2	- Second character of Q-P representation
 *
 * Returns the decoded value, -1 if the conversion failed.
 */
int decode_qp(unsigned char byte1, unsigned char byte2);

int default_done (int);

/*
 * Encode a message header using RFC 2047 encoding.  If the message contains
 * no non-ASCII characters, then leave the header as-is.
 *
 * Arguments include:
 *
 * name		- Message header name
 * value	- Message header content; must point to allocated memory
 *		  (may be changed if encoding is necessary)
 * encoding	- Encoding type.  May be one of CE_UNKNOWN (function chooses
 *		  the encoding), CE_BASE64 or CE_QUOTED
 * charset	- Charset used for encoding.  If NULL, obtain from system
 *		  locale.
 *
 * Returns 0 on success, any other value on failure.
 */

int encode_rfc2047(const char *name, char **value, int encoding,
		   const char *charset);

void escape_display_name (char *, size_t);
void escape_local_part (char *, size_t);
int ext_hook(char *, char *, char *);
int fdcompare (int, int);
int folder_addmsg (struct msgs **, char *, int, int, int, int, char *);
int folder_delmsgs (struct msgs *, int, int);
void folder_free (struct msgs *);
int folder_pack (struct msgs **, int);

/*
 * Read a MH folder structure and return an allocated "struct msgs"
 * corresponding to the contents of the folder.
 *
 * Arguments include:
 *
 * name		- Name of folder
 * lockflag	- If true, write-lock (and keep open) metadata files.
 *		  See comments for seq_read() for more information.
 */
struct msgs *folder_read (char *name, int lockflag);

struct msgs *folder_realloc (struct msgs *, int, int);
int gans (char *, struct swit *);
char **getans (char *, struct swit *);
#ifdef READLINE_SUPPORT
char **getans_via_readline (char *, struct swit *);
#endif /* READLINE_SUPPORT */
int getanswer (char *);
char **getarguments (char *, int, char **, int);

/*
 * Returns the MIME character set indicated by the current locale setting.
 * Should be used by routines that want to convert to/from the local
 * character set, or if you want to check to see if you can display content
 * in the local character set.
 */
char *get_charset(void);

char *getcpy (const char *);
char *get_default_editor(void);
char *getfolder(int);

/*
 * Get a string from the terminfo database for the current terminal.
 *
 * Retrieve the specified terminfo capability and return a string that
 * can be output to the terminal.  The string returned has already been
 * processed by tputs(), so it is safe to output directly.  The return
 * value of this function is valid until the next call.
 *
 * Arguments:
 *
 * capability	- The name of the terminfo capability (see terminfo(5)).
 *
 * Returns a tputs-processed string, or NULL if terminal initialization failed
 * or the capability wasn't found.
 */
char *get_term_stringcap(char *capability);

/*
 * Get a parameterized string from the terminfo database for the current
 * terminal.
 *
 * We don't yet have a standardized tparm() that will take a stdarg
 * argument.  Right now we don't want many parameters, so we only
 * take two.  Everything gets passed to tparm() as-is.  If we need
 * a capability with more arguments, we'll just add more later.
 *
 * Arguments:
 *
 * capability	- The name of the terminfo capability (see terminfo(5)).
 * arg1..argN	- Arguments 1-N.
 *
 * Returns a tparm and tputs-processed string, or NULL if there was a problem
 * initializating the terminal or retrieving the capability.
 */
char *get_term_stringparm(char *capability, long arg1, long arg2);

/*
 * Get a number from the terminfo database for the current terminal.
 *
 * Retrieve the specified terminfo capability and return the numeric
 * value of that capability from the terminfo database.
 *
 * Arguments:
 *
 * capability	- The name of the terminfo capability (see terminfo(5)).
 *
 * Returns the output of tigetnum() for that capability, or -1 if it was
 * unable to initialize the terminfo database.
 */
int get_term_numcap(char *capability);

/*
 * Lock open/close routines.
 *
 * The lk[f]opendata() functions are designed to open "data" files (anything
 * not a mail spool file) using the locking mechanism configured for data
 * files.  The lk[f]openspool() functions are for opening the mail spool
 * file, which will use the locking algorithm configured for the mail
 * spool.
 *
 * Files opened for reading are locked with a read lock (if possible by
 * the underlying lock mechanism), files opened for writing are locked
 * using an exclusive lock.  The int * argument is used to indicate failure
 * to acquire a lock.
 */
int lkclosedata(int, const char *);
int lkclosespool(int, const char *);
int lkfclosedata(FILE *, const char *);
int lkfclosespool(FILE *, const char *);
FILE *lkfopendata(const char *, const char *, int *);
int lkopendata(const char *, int, mode_t, int *);
FILE *lkfopenspool(const char *, const char *);
int lkopenspool(const char *, int, mode_t, int *);
int m_atoi (char *);
char *m_backup (char *);
int m_convert (struct msgs *, char *);
char *m_draft (char *, char *, int, int *);
void m_getfld_state_reset (m_getfld_state_t *);
void m_getfld_state_destroy (m_getfld_state_t *);
void m_getfld_track_filepos (m_getfld_state_t *, FILE *);
int m_getfld (m_getfld_state_t *, char[NAMESZ], char *, int *, FILE *);
int m_gmprot (void);
char *m_maildir (char *);
char *m_mailpath (char *);
char *m_name (int);
int m_putenv (char *, char *);
int m_rand (unsigned char *, size_t);

/*
 * Create a subprocess and redirect our standard output to it.
 *
 * Arguments are:
 *
 * name		- Name of process to create
 * savestdout	- If true, will save the current stdout file descriptor and
 *		  m_pclose() will close it at the appropriate time.
 */
void m_popen(char *name, int savestdout);

/*
 * Wait for the last process opened by m_popen().
 */
void m_pclose(void);

void m_unknown(m_getfld_state_t *, FILE *);
int makedir (char *);
char *message_id (time_t, int);

/*
 * Return a MIME content-type string for the specified file.
 *
 * If the system supports it, will use the "file" command to determine
 * the appropriate content-type.  Otherwise it will try to determine the
 * content-type from the suffix.  If that fails, the file will be scanned
 * and either assigned a MIME type of text/plain or application/octet-stream
 * depending if binary content is present.
 *
 * Arguments:
 *
 * filename	- The name of the file to determine the MIME type of.
 *
 * Returns a pointer to a content-type string (which may include MIME
 * parameters, such as charset).  Returns a NULL if it cannot determine
 * the MIME type of the file.  Returns allocated storage that must be
 * free'd.
 */
char *mime_type(const char *filename);

/*
 * Clear the screen, using the appropriate entry from the terminfo database
 */
void nmh_clear_screen(void);
char *nmh_getpass(const char *);
char *new_fs (char *, char *, char *);
char *path(char *, int);
int peekc(FILE *ib);
int pidwait (pid_t, int);
int pidstatus (int, FILE *, char *);
char *pluspath(char *);
void print_help (char *, struct swit *, int);
void print_sw (char *, struct swit *, char *, FILE *);
void print_version (char *);
void push (void);
char *pwd (void);
char *r1bindex(char *, int);
void readconfig (struct node **, FILE *, char *, int);
int refile (char **, char *);
void ruserpass (char *, char **, char **);
int remdir (char *);
void scan_detect_mbox_style (FILE *);
void scan_finished ();
void scan_reset_m_getfld_state ();
int seq_addmsg (struct msgs *, char *, int, int, int);
int seq_addsel (struct msgs *, char *, int, int);
char *seq_bits (struct msgs *);
int seq_delmsg (struct msgs *, char *, int);
int seq_delsel (struct msgs *, char *, int, int);
int seq_getnum (struct msgs *, char *);
char *seq_list (struct msgs *, char *);
int seq_nameok (char *);
void seq_print (struct msgs *, char *);
void seq_printall (struct msgs *);

/*
 * Read the sequence files for the folder referenced in the given
 * struct msgs and populate the sequence entries in the struct msgs.
 *
 * Arguments:
 *
 * mp		- Folder structure to add sequence entries to
 * lockflag	- If true, obtain a write lock on the sequence file.
 *		  Additionally, the sequence file will remain open
 *		  and a pointer to the filehandle will be stored in
 *		  folder structure, where it will later be used by
 *		  seq_save().
 *
 * Return values:
 *     OK       - successfully read the sequence files, or they don't exist
 *     NOTOK    - failed to lock sequence file
 */
int seq_read (struct msgs * mp, int lockflag);
void seq_save (struct msgs *);
void seq_setcur (struct msgs *, int);
void seq_setprev (struct msgs *);
void seq_setunseen (struct msgs *, int);
int showfile (char **, char *);
int smatch(char *, struct swit *);

/*
 * Convert a set of bit flags to printable format.
 *
 * Arguments:
 *
 * buffer	- Buffer to output string to.
 * size		- Size of buffer in bytes.  Buffer is always NUL terminated.
 * flags	- Binary flags to output
 * bitfield	- Textual representation of bits to output.  This string
 *		  is in the following format:
 *
 *	Option byte 0x01 STRING1 0x02 STRING2 ....
 *
 * The first byte is an option byte to snprintb().  Currently the only option
 * supported is 0x08, which indicates that the flags should be output in
 * octal format; if the option byte is any other value, the flags will be
 * output in hexadecimal.
 *
 * After the option bytes are series of text strings, prefixed by the number
 * of the bit they correspond to.  For example, the bitfield string:
 *
 *	"\020\01FLAG1\02FLAG2\03FLAG3\04FLAG4"
 *
 * will output the following string if "flags" is set to 0x09:
 *
 *	0x2<FLAG1,FLAG4>
 *
 * You don't have to use octal in the bitfield string, that's just the
 * convention currently used by the nmh code.  The order of flags in the
 * bitfield string is not significant, but again, general convention is
 * from least significant bit to most significant.
 */
char *snprintb (char *buffer, size_t size, unsigned flags, char *bitfield);
int ssequal (char *, char *);
int stringdex (char *, char *);
char *trimcpy (char *);
int unputenv (char *);

/*
 * Remove quotes and quoted-pair sequences from RFC-5322 atoms.
 *
 * Currently the actual algorithm is simpler than it technically should
 * be: any quotes are simply eaten, unless they're preceded by the escape
 * character (\).  This seems to be sufficient for our needs for now.
 *
 * Arguments:
 *
 * input	- The input string
 * output	- The output string; is assumed to have at least as much
 *		  room as the input string.  At worst the output string will
 *		  be the same size as the input string; it might be smaller.
 *
 */
void unquote_string(const char *input, char *output);
int uprf (char *, char *);
int vfgets (FILE *, char **);

/*
 * Output the local character set name, but make sure it is suitable for
 * 8-bit characters.
 */
char *write_charset_8bit (void);


/*
 * some prototypes for address parsing system
 * (others are in addrsbr.h)
 */
char *LocalName(int);
char *SystemName(void);

/*
 * prototypes for some routines in uip
 */
int annotate (char *, char *, char *, int, int, int, int);
void annolist(char *, char *, char *, int);
void annopreserve(int);
void m_pclose(void);
int make_intermediates(char *);
int mhl(int, char **);
int mhlsbr(int, char **, FILE *(*)(char *));
int distout (char *, char *, char *);
void replout (FILE *, char *, char *, struct msgs *, int,
	int, char *, char *, char *, int);
int sc_length(void);
int sc_width(void);
int build_form (char *, char *, int *, char *, char *, char *, char *,
		char *, char *);
int sendsbr (char **, int, char *, char *, struct stat *, int);
int SOprintf (char *, ...);
int what_now (char *, int, int, char *, char *,
	int, struct msgs *, char *, int, char *, int);
int WhatNow(int, char **);

/*
 * Copy data from one file to another, converting to base64-encoding.
 *
 * Arguments include:
 *
 * in		- Input filehandle (unencoded data)
 * out		- Output filename (base64-encoded data)
 * crlf		- If set, output encoded CRLF for every LF on input.
 *
 * Returns OK on success, NOTOK otherwise.
 */
int writeBase64aux(FILE *in, FILE *out, int crlf);

int writeBase64 (unsigned char *, size_t, unsigned char *);
int writeBase64raw (unsigned char *, size_t, unsigned char *);

/*
 * credentials management
 */
void init_credentials_file ();
int nmh_get_credentials (char *, char *, int, nmh_creds_t);

/*
 * temporary file management
 */
int nmh_init(const char *argv0, int read_context);
int m_unlink(const char *);
void unregister_for_removal(int remove_files);


/*
 * old h/nmh.h
 */

# include <dirent.h>
#define NLENGTH(dirent) strlen((dirent)->d_name)

/*
 * we should be getting this value from pathconf(_PC_PATH_MAX)
 */
#ifndef PATH_MAX
# ifdef MAXPATHLEN
#  define PATH_MAX MAXPATHLEN
# else
   /* so we will just pick something */
#  define PATH_MAX 1024
# endif
#endif

/*
 * we should get this value from sysconf(_SC_NGROUPS_MAX)
 */
#ifndef NGROUPS_MAX
# ifdef NGROUPS
#  define NGROUPS_MAX NGROUPS
# else
#  define NGROUPS_MAX 16
# endif
#endif

/*
 * we should be getting this value from sysconf(_SC_OPEN_MAX)
 */
#ifndef OPEN_MAX
# ifdef NOFILE
#  define OPEN_MAX NOFILE
# else
   /* so we will just pick something */
#  define OPEN_MAX 64
# endif
#endif

/*
 * Defaults for programs if they aren't configured in a user's profile
 */

#define DEFAULT_PAGER "more"
#define DEFAULT_EDITOR "vi"
