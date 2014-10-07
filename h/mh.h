/*
 * mh.h -- main header file for all of nmh
 */

/*
 * XXX
 *
 * When compiling with _XOPEN_SOURCE=600, Solaris 11 does not bring
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


/*
 * addrsbr.h -- definitions for the address parsing system
 */

#define	UUCPHOST	(-1)
#define	LOCALHOST	0
#define	NETHOST		1
#define	BADHOST		2

/*
 * The email structure used by nmh to define an email address
 */

struct mailname {
    struct mailname *m_next;	/* Linked list linkage; available for */
				/* application use */
    char *m_text;		/* Full unparsed text of email address */
    char *m_pers;		/* display-name in RFC 5322 parlance */
    char *m_mbox;		/* local-part in RFC 5322 parlance */
    char *m_host;		/* domain in RFC 5322 parlance */
    char *m_path;		/* Host routing; should not be used */
    int m_type;			/* UUCPHOST, LOCALHOST, NETHOST, or BADHOST */
    char m_nohost;		/* True if no host part available */
    char m_bcc;			/* Used by post to keep track of bcc's */
    int m_ingrp;		/* True if email address is in a group */
    char *m_gname;		/* display-name of group */
    char *m_note;		/* Note (post-address comment) */
};

/*
 * See notes for auxformat() below.
 */

#define	adrformat(m) auxformat ((m), 1)

/*
 *  prototypes
 */
void mnfree(struct mailname *);
int ismymbox(struct mailname *);

/*
 * Parse an address header, and return a sequence of email addresses.
 * This function is the main entry point into the nmh address parser.
 * It is used in conjunction with getm() to parse an email header.
 *
 * Arguments include:
 *
 * header	- Pointer to the start of an email header.
 *
 * On the first call, header is copied and saved internally.  Each email
 * address in the header is returned on the first and subsequent calls
 * to getname().  When there are no more email addresses available in
 * the header, NULL is returned and the parser's internal state is
 * reset.
 */

char *getname(const char *header);

/*
 * Format an email address given a struct mailname.
 *
 * This function takes a pointer to a struct mailname and returns a pointer
 * to a static buffer holding the resulting email address.
 *
 * It is worth noting that group names are NOT handled, so if you want to
 * do something with groups you need to handle it externally to this function.
 *
 * Arguments include:
 *
 * mp		- Pointer to mailname structure
 * extras	- If true, include the personal name and/or note in the
 *		  address.  Otherwise, omit it.
 */

char *auxformat(struct mailname *mp, int extras);

/*
 * Parse an email address into it's components.
 *
 * Used in conjunction with getname() to parse a complete email header.
 *
 * Arguments include:
 *
 * str		- Email address being parsed.
 * dfhost	- A default host to append to the email address if
 *		  one is not included.  If NULL, use nmh's idea of
 *		  localhost().
 * dftype	- If dfhost is given, use dftype as the email address type
 *		  if no host is in the email address.
 * eresult	- A buffer containing an error message returned by the
 *		  address parser.  May be NULL.
 * eresultsize	- The size of the buffer passed in eresult.
 *
 * A pointer to an allocated struct mailname corresponding to the email
 * address is returned.
 *
 * This function used to have an argument called 'wanthost' which would
 * control whether or not it would canonicalize hostnames in email
 * addresses.  This functionalit was removed for nmh 1.5, and eventually
 * all of the code that used this argument was garbage collected.
 */
struct mailname *getm(char *str, char *dfhost, int dftype, char *eresult,
		      size_t eresultsize);


/*
 * aliasbr.h -- definitions for the aliasing system
 *
 */

extern char *AliasFile;		/* mh-alias(5)             */

struct aka {
    char *ak_name;		/* name to match against             */
    struct adr *ak_addr;	/* list of addresses that it maps to */
    struct aka *ak_next;	/* next aka in list                  */
    char ak_visible;		/* should be visible in headers      */
};

struct adr {
    char *ad_text;		/* text of this address in list        */
    struct adr *ad_next;	/* next adr in list                    */
    char ad_local;		/* text is local (check for expansion) */
};

/*
 * incore version of /etc/passwd
 */
struct home {
    char *h_name;		/* user name                             */
    uid_t h_uid;		/* user id                               */
    gid_t h_gid;		/* user's group                          */
    char *h_home;		/* user's home directory                 */
    char *h_shell;		/* user's shell                          */
    int	h_ngrps;		/* number of groups this user belongs to */
    struct home *h_next;	/* next home in list                     */
};

/*
 * prototypes
 */
int alias (char *);
int akvisible (void);
void init_pw (void);
char *akresult (struct aka *);
char *akvalue (char *);
char *akerror (int);

/* codes returned by alias() */

#define	AK_OK		0	/* file parsed ok 	 */
#define	AK_NOFILE	1	/* couldn't read file 	 */
#define	AK_ERROR	2	/* error parsing file 	 */
#define	AK_LIMIT	3	/* memory limit exceeded */
#define	AK_NOGROUP	4	/* no such group 	 */



/*
 * mts.h -- definitions for the mail system
 */

/*
 * Local and UUCP Host Name
 */
char *LocalName(int);
char *SystemName(void);

/*
 * Mailboxes
 */
extern char *mmdfldir;
extern char *mmdflfil;
extern char *uucpldir;
extern char *uucplfil;
extern char *spoollocking;

#define	MAILDIR	(mmdfldir && *mmdfldir ? mmdfldir : getenv ("HOME"))
#define	MAILFIL	(mmdflfil && *mmdflfil ? mmdflfil : getusername ())

char *getusername(void);
char *getfullname(void);
char *getlocalmbox(void);

/*
 * Separators
 */
extern char *mmdlm1;
extern char *mmdlm2;

#define	isdlm1(s) (strcmp (s, mmdlm1) == 0)
#define	isdlm2(s) (strcmp (s, mmdlm2) == 0)

/*
 * Read mts.conf file
 */
void mts_init (char *);

/*
 * MTS specific variables
 */

/* whether to speak SMTP, and over the network or directly to sendmail */
#define MTS_SMTP          0
#define MTS_SENDMAIL_SMTP 1
#define MTS_SENDMAIL_PIPE 2
extern int sm_mts;
void save_mts_method (const char *);

extern char *sendmail;

/*
 * SMTP/POP stuff
 */
extern char *clientname;
extern char *servers;
extern char *pophost;

/*
 * Global MailDelivery File
 */
extern char *maildelivery;



/*
 * utils.h -- utility prototypes
 */

void *mh_xmalloc(size_t);
void *mh_xrealloc(void *, size_t);
char *pwd(void);
char *add(const char *, char *);
char *addlist(char *, const char *);
int folder_exists(const char *);
void create_folder(char *, int, void (*)(int));
int num_digits(int);

/*
 * A vector of char array, used to hold a list of string message numbers
 * or command arguments.
 */

struct msgs_array {
	int max, size;
	char **msgs;
};

/*
 * Same as msgs_array, but for a vector of ints
 */

struct msgnum_array {
	int max, size;
	int *msgnums;
};

/*
 * Add a argument to the given msgs_array or msgnum_array structure; extend
 * the array size if necessary
 */

void app_msgarg(struct msgs_array *, char *);
void app_msgnum(struct msgnum_array *, int);

int open_form(char **, char *);
char *find_str (const char [], size_t, const char *);
char *rfind_str (const char [], size_t, const char *);
char *nmh_strcasestr (const char *, const char *);

/*
 * See if a string contains 8 bit characters (use isascii() for the test).
 * Arguments include:
 *
 * start	- Pointer to start of string to test.
 * end		- End of string to test (test will stop before reaching
 *		  this point).  If NULL, continue until reaching '\0'.
 *
 * This function always stops at '\0' regardless of the value of 'end'.
 * Returns 1 if the string contains an 8-bit character, 0 if it does not.
 */
int contains8bit(const char *start, const char *end);




/*
 * mhparse.h -- definitions for parsing/building of MIME content
 *           -- (mhparse.c/mhbuildsbr.c)
 */

#define	NPARTS	50
#define	NTYPES	20
#define	NPARMS	10

/*
 * Abstract type for header fields
 */
typedef struct hfield *HF;

/*
 * Abstract types for MIME parsing/building
 */
typedef struct cefile		*CE;
typedef struct CTinfo		*CI;
typedef struct Content		*CT;
typedef struct Parameter	*PM;

/*
 * type for Init function (both type and transfer encoding)
 */
typedef int (*InitFunc) (CT);

/*
 * types for various transfer encoding access functions
 */
typedef int (*OpenCEFunc) (CT, char **);
typedef void (*CloseCEFunc) (CT);
typedef unsigned long (*SizeCEFunc) (CT);

/*
 * Structure for storing/encoding/decoding
 * a header field and its value.
 */
struct hfield {
    char *name;		/* field name */
    char *value;	/* field body */
    int hf_encoding;	/* internal flag for transfer encoding to use */
    HF next;		/* link to next header field */
};

/*
 * Structure for holding MIME parameter elements.
 */
struct Parameter {
    char *pm_name;	/* Parameter name */
    char *pm_value;	/* Parameter value */
    char *pm_charset;	/* Parameter character set (optional) */
    char *pm_lang;	/* Parameter language tag (optional) */
    PM   pm_next;	/* Pointer to next element */
};

/*
 * Structure for storing parsed elements
 * of the Content-Type component.
 */
struct CTinfo {
    char *ci_type;		/* content type     */
    char *ci_subtype;		/* content subtype  */
    PM   ci_first_pm;		/* Pointer to first MIME parameter */
    PM   ci_last_pm;		/* Pointer to last MIME parameter */
    char *ci_comment;		/* RFC-822 comments */
    char *ci_magic;
};

/*
 * Structure for storing decoded contents after
 * removing Content-Transfer-Encoding.
 */
struct cefile {
    char *ce_file;	/* decoded content (file)   */
    FILE *ce_fp;	/* decoded content (stream) */
    int	  ce_unlink;	/* remove file when done?   */
};

/*
 * Primary structure for handling Content (Entity)
 *
 * Some more explanation of this:
 *
 * This structure recursively describes a complete MIME message.
 * At the top level, the c_first_hf list has a list of all message
 * headers.  If the content-type is multipart (c_type == CT_MULTIPART)
 * then c_ctparams will contain a pointer to a struct multipart.
 * A struct multipart contains (among other trhings) a linked list
 * of struct part elements, and THOSE contain a pointer to the sub-part's
 * Content structure.
 */
struct Content {
    /* source (read) file */
    char *c_file;		/* read contents (file)              */
    FILE *c_fp;			/* read contents (stream)            */
    int	c_unlink;		/* remove file when done?            */

    long c_begin;		/* where content body starts in file */
    long c_end;			/* where content body ends in file   */

    /* linked list of header fields */
    HF c_first_hf;		/* pointer to first header field     */
    HF c_last_hf;		/* pointer to last header field      */

    /* copies of MIME related header fields */
    char *c_vrsn;		/* MIME-Version:                     */
    char *c_ctline;		/* Content-Type:                     */
    char *c_celine;		/* Content-Transfer-Encoding:        */
    char *c_id;			/* Content-ID:                       */
    char *c_descr;		/* Content-Description:              */
    char *c_dispo;		/* Content-Disposition:              */
    char *c_dispo_type;		/* Type of Content-Disposition       */
    PM c_dispo_first;		/* Pointer to first disposition parm */
    PM c_dispo_last;		/* Pointer to last disposition parm  */
    char *c_partno;		/* within multipart content          */

    /* Content-Type info */
    struct CTinfo c_ctinfo;	/* parsed elements of Content-Type   */
    int	c_type;			/* internal flag for content type    */
    int	c_subtype;		/* internal flag for content subtype */

    /* Content-Transfer-Encoding info (decoded contents) */
    struct cefile c_cefile;	/* structure holding decoded content */
    int	c_encoding;		/* internal flag for encoding type   */
    int c_reqencoding;		/* Requested encoding (by mhbuild)   */

    /* Content-MD5 info */
    int	c_digested;		/* have we seen this header before?  */
    unsigned char c_digest[16];	/* decoded MD5 checksum              */

    /* pointers to content-specific structures */
    void *c_ctparams;		/* content type specific data        */
    struct exbody *c_ctexbody;	/* data for type message/external    */

    /* function pointers */
    InitFunc    c_ctinitfnx;	/* parse content body                */
    OpenCEFunc  c_ceopenfnx;	/* get a stream to decoded contents  */
    CloseCEFunc c_ceclosefnx;	/* release stream                    */
    SizeCEFunc  c_cesizefnx;	/* size of decoded contents          */

    int	c_umask;		/* associated umask                  */
    int	c_rfc934;		/* rfc934 compatibility flag         */

    char *c_showproc;		/* default, if not in profile        */
    char *c_termproc;		/* for charset madness...            */
    char *c_storeproc;		/* overrides profile entry, if any   */

    char *c_storage;		/* write contents (file)             */
    char *c_folder;		/* write contents (folder)           */
};

/*
 * Flags for Content-Type (Content->c_type)
 */
#define	CT_UNKNOWN	0x00
#define	CT_APPLICATION	0x01
#define	CT_AUDIO	0x02
#define	CT_IMAGE	0x03
#define	CT_MESSAGE	0x04
#define	CT_MULTIPART	0x05
#define	CT_TEXT		0x06
#define	CT_VIDEO	0x07
#define	CT_EXTENSION	0x08

/*
 * Flags for Content-Transfer-Encoding (Content->c_encoding)
 */
#define	CE_UNKNOWN	0x00
#define	CE_BASE64	0x01
#define	CE_QUOTED	0x02
#define	CE_8BIT		0x03
#define	CE_7BIT		0x04
#define	CE_BINARY	0x05
#define	CE_EXTENSION	0x06
#define	CE_EXTERNAL	0x07	/* for external-body */

/*
 * TEXT content
 */

/* Flags for subtypes of TEXT */
#define	TEXT_UNKNOWN	0x00
#define	TEXT_PLAIN	0x01
#define	TEXT_RICHTEXT	0x02
#define TEXT_ENRICHED	0x03

/* Flags for character sets */
#define	CHARSET_SPECIFIED    0x00
#define CHARSET_UNSPECIFIED 0x01  /* only needed when building drafts */

/* Structure for text content */
struct text {
    int	tx_charset;		/* flag for character set */
};

/*
 * MULTIPART content
 */

/* Flags for subtypes of MULTIPART */
#define	MULTI_UNKNOWN	0x00
#define	MULTI_MIXED	0x01
#define	MULTI_ALTERNATE	0x02
#define	MULTI_DIGEST	0x03
#define	MULTI_PARALLEL	0x04

/* Structure for subparts of a multipart content */
struct part {
    CT mp_part;			/* Content structure for subpart     */
    struct part *mp_next;	/* pointer to next subpart structure */
};

/* Main structure for multipart content */
struct multipart {
    char *mp_start;		/* boundary string separating parts   */
    char *mp_stop;		/* terminating boundary string        */
    char *mp_content_before;	/* any content before the first subpart */
    char *mp_content_after;	/* any content after the last subpart */
    struct part *mp_parts;	/* pointer to first subpart structure */
};

/*
 * MESSAGE content
 */

/* Flags for subtypes of MESSAGE */
#define	MESSAGE_UNKNOWN	 0x00
#define	MESSAGE_RFC822	 0x01
#define	MESSAGE_PARTIAL	 0x02
#define	MESSAGE_EXTERNAL 0x03

/* Structure for message/partial */
struct partial {
    char *pm_partid;
    int	pm_partno;
    int	pm_maxno;
    int	pm_marked;
    int	pm_stored;
};

/* Structure for message/external */
struct exbody {
    CT eb_parent;	/* pointer to controlling content structure */
    CT eb_content;	/* pointer to internal content structure    */
    char *eb_partno;
    char *eb_access;
    int	eb_flags;
    char *eb_name;
    char *eb_permission;
    char *eb_site;
    char *eb_dir;
    char *eb_mode;
    unsigned long eb_size;
    char *eb_server;
    char *eb_subject;
    char *eb_body;
    char *eb_url;
};

/*
 * APPLICATION content
 */

/* Flags for subtype of APPLICATION */
#define	APPLICATION_UNKNOWN	0x00
#define	APPLICATION_OCTETS	0x01
#define	APPLICATION_POSTSCRIPT	0x02


/*
 * Structures for mapping types to their internal flags
 */
struct k2v {
    char *kv_key;
    int	  kv_value;
};
extern struct k2v SubText[];
extern struct k2v Charset[];
extern struct k2v SubMultiPart[];
extern struct k2v SubMessage[];
extern struct k2v SubApplication[];

/*
 * Structures for mapping (content) types to
 * the functions to handle them.
 */
struct str2init {
    char *si_key;
    int	  si_val;
    InitFunc si_init;
};
extern struct str2init str2cts[];
extern struct str2init str2ces[];
extern struct str2init str2methods[];

/*
 * prototypes
 */
CT parse_mime (char *);

/*
 * Translate a composition file into a MIME data structure.  Arguments are:
 *
 * infile	- Name of input filename
 * autobuild    - A flag to indicate if the composition file parser is
 *		  being run in automatic mode or not.  In auto mode,
 *		  if a MIME-Version header is encountered it is assumed
 *		  that the composition file is already in MIME format
 *		  and will not be processed further.  Otherwise, an
 *		  error is generated.
 * dist		- A flag to indicate if we are being run by "dist".  In
 *		  that case, add no MIME headers to the message.  Existing
 *		  headers will still be encoded by RFC 2047.
 * directives	- A flag to control whether or not build directives are
 *		  processed by default.
 * encoding	- The default encoding to use when doing RFC 2047 header
 *		  encoding.  Must be one of CE_UNKNOWN, CE_BASE64, or
 *		  CE_QUOTED.
 * maxunencoded	- The maximum line length before the default encoding for
 *		  text parts is quoted-printable.
 * verbose	- If 1, output verbose information during message composition
 *
 * Returns a CT structure describing the resulting MIME message.  If the
 * -auto flag is set and a MIME-Version header is encountered, the return
 * value is NULL.
 */
CT build_mime (char *infile, int autobuild, int dist, int directives,
	       int encoding, size_t maxunencoded, int verbose);

int add_header (CT, char *, char *);
int get_ctinfo (char *, CT, int);
int params_external (CT, int);
int open7Bit (CT, char **);
void close_encoding (CT);
void free_content (CT);
char *ct_type_str (int);
char *ct_subtype_str (int, int);
const struct str2init *get_ct_init (int);
const char *ce_str (int);
const struct str2init *get_ce_method (const char *);
char *content_charset (CT);
int convert_charset (CT, char *, int *);
void reverse_alternative_parts (CT);

/*
 * Given a content structure, return true if the content has a disposition
 * of "inline".
 *
 * Arguments are:
 *
 * ct		- Content structure to examine
 */
int is_inline(CT ct);

/*
 * Given a list of messages, display information about them on standard
 * output.
 *
 * Arguments are:
 *
 * cts		- An array of CT elements of messages that need to be
 *		  displayed.  Array is terminated by a NULL.
 * headsw	- If 1, display a column header.
 * sizesw	- If 1, display the size of the part.
 * verbosw	- If 1, display verbose information
 * debugsw	- If 1, turn on debugging for the output.
 * disposw	- If 1, display MIME part disposition information.
 *
 */
void list_all_messages(CT *cts, int headsw, int sizesw, int verbosw,
		       int debugsw, int disposw);

/*
 * List the content information of a single MIME part on stdout.
 *
 * Arguments are:
 *
 * ct		- MIME Content structure to display.
 * toplevel	- If set, we're at the top level of a message
 * realsize	- If set, determine the real size of the content
 * verbose	- If set, output verbose information
 * debug	- If set, turn on debugging for the output
 * dispo	- If set, display MIME part disposition information.
 *
 * Returns OK on success, NOTOK otherwise.
 */
int list_content(CT ct, int toplevel, int realsize, int verbose, int debug,
		 int dispo);

/*
 * Display content-appropriate information on MIME parts, decending recursively
 * into multipart content if appropriate.  Uses list_content() for displaying
 * generic information.
 *
 * Arguments and return value are the same as list_content().
 */
int list_switch(CT ct, int toplevel, int realsize, int verbose, int debug,
		int dispo);

/*
 * Given a linked list of parameters, build an output string for them.  This
 * string is designed to be concatenated on an already-built header.
 *
 * Arguments are:
 *
 * initialwidth	- Current width of the header.  Used to compute when to wrap
 *		  parameters on the first line.  The following lines will
 *		  be prefixed by a tab (\t) character.
 * params	- Pointer to head of linked list of parameters.
 * offsetout	- The final line offset after all the parameters have been
 *		  output.  May be NULL.
 * external	- If set, outputting an external-body type and will not
 *		  output a "body" parameter.
 
 * Returns a pointer to the resulting parameter string.  This string must
 * be free()'d by the caller.  Returns NULL on error.
 */
char *output_params(size_t initialwidth, PM params, int *offsetout,
		    int external);

/*
 * Add a parameter to the parameter linked list.
 *
 * Arguments are:
 *
 * first	- Pointer to head of linked list
 * last		- Pointer to tail of linked list
 * name		- Name of parameter
 * value	- Value of parameter
 * nocopy	- If set, will use the pointer values directly for "name"
 *		  and "value" instead of making their own copy.  These
 *		  pointers will be free()'d later by the MIME routines, so
 *		  they should not be used after calling this function!
 *
 * Returns allocated parameter element
 */
PM add_param(PM *first, PM *last, char *name, char *value, int nocopy);

/*
 * Replace (or add) a parameter to the parameter linked list.
 *
 * If the named parameter already exists on the parameter linked list,
 * replace the value with the new one.  Otherwise add it to the linked
 * list.  All parameters are identical to add_param().
 */
PM replace_param(PM *first, PM *last, char *name, char *value, int nocopy);

/*
 * Retrieve a parameter value from a parameter linked list.  Convert to the
 * local character set if required.
 *
 * Arguments are:
 *
 * first	- Pointer to head of parameter linked list.
 * name		- Name of parameter.
 * replace	- If characters in the parameter list cannot be converted to
 *		  the local character set, replace with this character.
 * fetchonly	- If true, return pointer to original value, no conversion
 *		  performed.
 *
 * Returns parameter value if found, NULL otherwise.  Memory must be free()'d
 * unless fetchonly is set.
 */

char *get_param(PM first, const char *name, char replace, int fetchonly);

/*
 * Fetch a parameter value from a parameter structure, converting it to
 * the local character set.
 *
 * Arguments are:
 *
 * pm		- Pointer to parameter structure
 * replace	- If characters in the parameter list cannot be converted to
 *		  the local character set, replace with this character.
 *
 * Returns a pointer to the parameter value.  Memory is stored in an
 * internal buffer, so the returned value is only valid until the next
 * call to get_param_value() or get_param() (get_param() uses get_param_value()
 * internally).
 */
char *get_param_value(PM pm, char replace);

/*
 * Display MIME message(s) on standard out.
 *
 * Arguments are:
 *
 * cts		- NULL terminated array of CT structures for messages
 *		  to display
 * concat	- If true, concatenate all MIME parts.  If false, show each
 *		  MIME part under a separate pager.
 * textonly	- If true, only display "text" MIME parts
 * inlineonly	- If true, only display MIME parts that are marked with
 *		  a disposition of "inline" (includes parts that lack a
 *		  Content-Disposition header).
 * markerform	- The name of a file containg mh-format(5) code used to
 *		  display markers about non-displayed MIME parts.
 */
void show_all_messages(CT *cts, int concat, int textonly, int inlineonly,
		       char *markerform);

/*
 * Display (or store) a single MIME part using the specified command
 *
 * Arguments are:
 *
 * ct		- The Content structure of the MIME part we wish to display
 * alternate	- Set this to true if this is one part of a MIME
 *		  multipart/alternative part.  Will suppress some errors and
 *		  will cause the function to return DONE instead of OK on
 *		  success.
 * cp		- The command string to execute.  Will be run through the
 *		  parser for %-escapes as described in mhshow(1).
 * cracked	- If set, chdir() to this directory before executing the
 *		  command in "cp".  Only used by mhstore(1).
 * fmt		- A series of mh-format(5) instructions to execute if the
 *		  command string indicates a marker is desired.  Can be NULL.
 *
 * Returns NOTOK if we could not display the part, DONE if alternate was
 * set and we could display the part, and OK if alternate was not set and
 * we could display the part.
 */
struct format;
int show_content_aux(CT ct, int alternate, char *cp, char *cracked,
		     struct format *fmt);

extern int checksw;




/*
 * fmt_scan.h -- definitions for fmt_scan()
 */

/*
 * This structure describes an "interesting" component.  It holds
 * the name & text from the component (if found) and one piece of
 * auxilary info.  The structure for a particular component is located
 * by (open) hashing the name and using it as an index into the ptr array
 * "wantcomp".  All format entries that reference a particular component
 * point to its comp struct (so we only have to do component specific
 * processing once.  e.g., parse an address.).
 *
 * In previous implementations "wantcomp" was made available to other
 * functions, but now it's private and is accessed via functions.
 */
struct comp {
    char        *c_name;	/* component name (in lower case) */
    char        *c_text;	/* component text (if found)      */
    struct comp *c_next;	/* hash chain linkage             */
    short        c_flags;	/* misc. flags (from fmt_scan)    */
    short        c_type;	/* type info   (from fmt_compile) */
    union {
	struct tws *c_u_tws;
	struct mailname *c_u_mn;
    } c_un;
    int          c_refcount;	/* Reference count                */
};

#define c_tws c_un.c_u_tws
#define c_mn  c_un.c_u_mn

/*
 * c_type bits
 */
#define	CT_ADDR       (1<<0)	/* referenced as address    */
#define	CT_DATE       (1<<1)	/* referenced as date       */

#define CT_BITS       "\020\01ADDR\02DATE"

/*
 * c_flags bits
 */
#define	CF_TRUE       (1<<0)	/* usually means component is present */
#define	CF_PARSED     (1<<1)	/* address/date has been parsed */
#define	CF_DATEFAB    (1<<2)	/* datefield fabricated */
#define CF_TRIMMED    (1<<3)	/* Component has been trimmed */

#define CF_BITS       "\020\01TRUE\02PARSED\03CF_DATEFAB\04TRIMMED"

/*
 * This structure defines one formatting instruction.
 */
struct format {
    unsigned char f_type;
    char          f_fill;
    short         f_width;	/* output field width   */
    union {
	struct comp *f_u_comp;	/* associated component */
	char        *f_u_text;	/* literal text         */
	char         f_u_char;	/* literal character    */
	int          f_u_value;	/* literal value        */
    } f_un;
    short         f_flags;	/* misc. flags          */
};

#define f_skip f_width		/* instr to skip (false "if") */

#define f_comp  f_un.f_u_comp
#define f_text  f_un.f_u_text
#define f_char  f_un.f_u_char
#define f_value f_un.f_u_value

/*
 * f_flags bits
 */

#define FF_STRALLOC	(1<<0)	/* String has been allocated */
#define FF_COMPREF	(1<<1)	/* Component reference */

/*
 * prototypes used by the format engine
 */

/*
 * These are the definitions used by the callbacks for fmt_scan()
 */

typedef char * (*formataddr_cb)(char *, char *);
typedef char * (*concataddr_cb)(char *, char *);
typedef void (*trace_cb)(void *, struct format *, int, char *, const char *);

struct fmt_callbacks {
    formataddr_cb	formataddr;
    concataddr_cb	concataddr;
    trace_cb		trace_func;
    void *		trace_context;
};

/*
 * Create a new format string.  Arguments are:
 *
 * form		- Name of format file.  Will be searched by etcpath(), see that
 *		  function for details.
 * format	- The format string to be used if no format file is given
 * default_fs	- The default format string to be used if neither form nor
 *		  format is given
 *
 * This function also takes care of processing \ escapes like \n, \t, etc.
 *
 * Returns an allocated format string.
 */

char *new_fs (char *form, char *format, char *default_fs);

/*
 * Compile a format string into a set of format instructions.  Arguments are:
 *
 * fstring	- The format string (the "source code").
 * fmt		- Returns an allocated array of "struct fmt" elements.  Each
 *		  struct fmt is one format instruction interpreted by the
 *		  format engine.
 * reset	- If set to true, the format compiler will reset the
 *		  component hash table.  The component hash table contains
 *		  all of the references to message components refered to in
 *		  the format instructions.  If you have multiple format
 *		  strings that you want to compile and operate on the
 *		  same message, this should be set to false.
 *
 * Returns the total number of components referenced by all format instructions
 * since the last reset of the hash table.
 */

int fmt_compile (char *fstring, struct format **fmt, int reset);

/*
 * Interpret a sequence of compiled format instructions.  Arguments are:
 *
 * format	- Array of format instructions generated by fmt_compile()
 * scanl	- Passed-in charstring_t object (created with
 *		  charstring_create() and later destroyed with
 *		  charstring_free()) that will contain the output of the
 *		  format instructions.  Is always terminated with a
 *		  newline (\n).
 * width	- Maximum number of displayed characters.  Does not include
 *		  characters marked as nonprinting or (depending on the
 *		  encoding) bytes in a multibyte encoding that exceed the
 *		  character's column width.
 * dat		- An integer array that contains data used by certain format
 *		  functions.  Currently the following instructions use
 *		  dat[]:
 *
 *		dat[0] - %(msg), %(dat)
 *		dat[1] - %(cur)
 *		dat[2] - %(size)
 *		dat[3] - %(width)
 *		dat[4] - %(unseen)
 *
 * callbacks	- A set of a callback functions used by the format engine.
 *		  Can be NULL.  If structure elements are NULL, a default
 *		  function will be used.  Callback structure elements are:
 *
 *		formataddr	- A callback for the %(formataddr) instruction
 *		concataddr	- A callback for the %(concataddr) instruction
 *		trace		- Called for every format instruction executed
 *
 * The return value is a pointer to the next format instruction to
 * execute, which is currently always NULL.
 */

struct format *fmt_scan (struct format *format, charstring_t scanl, int width,
			 int *dat, struct fmt_callbacks *callbacks);

/*
 * Free a format structure and/or component hash table.  Arguments are:
 *
 * format	- An array of format structures allocated by fmt_compile,
 *		  or NULL.
 * reset	- If true, reset and remove all references in the component
 *		  hash table.
 */

void fmt_free (struct format *fmt, int reset);

/*
 * Free all of the component text structures in the component hash table
 */

void fmt_freecomptext(void);

/*
 * Search for a component structure in the component hash table.  Arguments are:
 *
 * component	- The name of the component to search for.  By convention
 *		  all component names used in format strings are lower case,
 *		  but for backwards compatibility this search is done in
 *		  a case-SENSITIVE manner.
 *
 * This function returns a "struct comp" corresponding to the named component,
 * or NULL if the component is not found in the hash table.
 */

struct comp *fmt_findcomp(char *component);

/*
 * Search for a component structure in the component hash table.
 *
 * Identical to fmd_findcomp(), but is case-INSENSITIVE.
 */

struct comp *fmt_findcasecomp(char *component);

/*
 * Add a component entry to the component hash table
 *
 * component	- The name of the component to add to the hash table.
 *
 * If the component is already in the hash table, this function will do
 * nothing.  Returns 1 if a component was added, 0 if it already existed.
 */

int fmt_addcompentry(char *component);

/*
 * Add a string to a component hash table entry.  Arguments are:
 *
 * component	- The name of the component to add text to.  The component
 *		  is searched for in a case-INSENSITIVE manner (note that
 *		  this is different than fmt_findcomp()).  If the component
 *		  is not found in the hash table, this function will silently
 *		  return.
 * text		- The text to add to a component hash table entry.  Note that
 *                if the last character of the existing component
 *                text is a newline AND it is marked as an address
 *                component (the the CT_ADDR flag is set) existing
 *                component buffer is a newline, it will be separated
 *                from previous text by ",\n\t"; otherwise if the last
 *                character of the previous text is a newline it will
 *                simply be seperated by a "\t".  This unusual processing
 *		  is designed to handle the case where you have multiple
 *		  headers with the same name (e.g.: multiple "cc:" headers,
 *		  even though that isn't technically allowed in the RFCs).
 *
 * This function is designed to be called when you start processing a new
 * component.  The function returns the integer value of the hash table
 * bucket corresponding to this component.  If there was no entry found
 * in the component hash table, this function will return -1.
 */

int fmt_addcomptext(char *component, char *text);

/*
 * Append to an existing component.  Arguments are:
 *
 * bucket	- The hash table bucket corresponding to this component,
 *		  as returned by fmt_addcomp().  If -1, this function will
 *		  return with no actions performed.
 * component	- The component to append text to.  Like fmt_addcomp, the
 *		  component is searched case-INSENSITIVELY.
 * text		- The text to append to the component.  No special processing
 *		  is done.
 *
 * This function is designed to be called when you are processing continuation
 * lines on the same header (state == FLDPLUS).
 */

void fmt_appendcomp(int bucket, char *component, char *text);

/*
 * Iterate over the complete hash table of component structures.
 *
 * Arguments are:
 *
 * comp		- Pointer to the current component structure.  The next
 *		  component in the hash table after this component.  To
 *		  start (or restart) the iteration of the hash table
 *		  this argument should be NULL.
 * bucket	- Pointer to hash bucket.  Will be managed by this function,
 *		  the caller should not modify this value.
 *
 * Returns the next component in the hash table.  This value should be
 * passed into the next call to fmt_nextcomp().  Returns NULL at the end
 * of the hash table.
 */

struct comp *fmt_nextcomp(struct comp *comp, unsigned int *bucket);

/*
 * The implementation of the %(formataddr) function.  This is available for
 * programs to provide their own local implementation if they wish to do
 * special processing (see uip/replsbr.c for an example).  Arguments are:
 *
 * orig		- Existing list of addresses
 * str		- New address(es) to append to list.
 *
 * This function returns an allocated string containing the new list of
 * addresses.
 */

char *formataddr(char *orig, char *str);

/*
 * The implementation of the %(concataddr) function.  Arguments and behavior
 * are the same as %(formataddr).  Again, see uip/replsbr.c to see how you
 * can override this behavior.
 */

char *concataddr(char *orig, char *str);




/*
 * fmt_compile.h -- format types
 */

/* types that output text */
#define FT_COMP         1       /* the text of a component                 */
#define FT_COMPF        2       /* comp text, filled                       */
#define FT_LIT          3       /* literal text                            */
#define FT_LITF         4       /* literal text, filled                    */
#define FT_CHAR         5       /* a single ascii character                */
#define FT_NUM          6       /* "value" as decimal number               */
#define FT_NUMF         7       /* "value" as filled dec number            */
#define FT_STR          8       /* "str" as text                           */
#define FT_STRF         9       /* "str" as text, filled                   */
#define FT_STRFW        10      /* "str" as text, filled, width in "value" */
#define FT_STRLIT       11      /* "str" as text, no space compression     */
#define FT_STRLITZ      12      /* literal text with zero display width    */
#define FT_PUTADDR      13      /* split and print address line            */

/* types that modify the "str" or "value" registers                     */
#define FT_LS_COMP      14      /* set "str" to component text          */
#define FT_LS_LIT       15      /* set "str" to literal text            */
#define FT_LS_GETENV    16      /* set "str" to getenv(text)            */
#define FT_LS_CFIND     17      /* set "str" to context_find(text)      */
#define FT_LS_DECODECOMP 18     /* set "str" to decoded component text  */
#define FT_LS_DECODE    19      /* decode "str" as RFC-2047 header      */
#define FT_LS_TRIM      20      /* trim trailing white space from "str" */
#define FT_LV_COMP      21      /* set "value" to comp (as dec. num)    */
#define FT_LV_COMPFLAG  22      /* set "value" to comp flag word        */
#define FT_LV_LIT       23      /* set "value" to literal num           */
#define FT_LV_DAT       24      /* set "value" to dat[n]                */
#define FT_LV_STRLEN    25      /* set "value" to length of "str"       */
#define FT_LV_PLUS_L    26      /* set "value" += literal               */
#define FT_LV_MINUS_L   27      /* set "value" -= literal               */
#define FT_LV_DIVIDE_L  28      /* set "value" to value / literal       */
#define FT_LV_MODULO_L  29      /* set "value" to value % literal       */
#define FT_LV_CHAR_LEFT 30      /* set "value" to char left in output   */

#define FT_LS_MONTH     31      /* set "str" to tws month                   */
#define FT_LS_LMONTH    32      /* set "str" to long tws month              */
#define FT_LS_ZONE      33      /* set "str" to tws timezone                */
#define FT_LS_DAY       34      /* set "str" to tws weekday                 */
#define FT_LS_WEEKDAY   35      /* set "str" to long tws weekday            */
#define FT_LS_822DATE   36      /* set "str" to 822 date str                */
#define FT_LS_PRETTY    37      /* set "str" to pretty (?) date str         */
#define FT_LS_KILO      38      /* set "str" to "<value>[KMGT]"             */
#define FT_LS_KIBI      39      /* set "str" to "<value>[KMGT]"             */
#define FT_LV_SEC       40      /* set "value" to tws second                */
#define FT_LV_MIN       41      /* set "value" to tws minute                */
#define FT_LV_HOUR      42      /* set "value" to tws hour                  */
#define FT_LV_MDAY      43      /* set "value" to tws day of month          */
#define FT_LV_MON       44      /* set "value" to tws month                 */
#define FT_LV_YEAR      45      /* set "value" to tws year                  */
#define FT_LV_YDAY      46      /* set "value" to tws day of year           */
#define FT_LV_WDAY      47      /* set "value" to tws weekday               */
#define FT_LV_ZONE      48      /* set "value" to tws timezone              */
#define FT_LV_CLOCK     49      /* set "value" to tws clock                 */
#define FT_LV_RCLOCK    50      /* set "value" to now - tws clock           */
#define FT_LV_DAYF      51      /* set "value" to tws day flag              */
#define FT_LV_DST       52      /* set "value" to tws daylight savings flag */
#define FT_LV_ZONEF     53      /* set "value" to tws timezone flag         */

#define FT_LS_PERS      54      /* set "str" to person part of addr    */
#define FT_LS_MBOX      55      /* set "str" to mbox part of addr      */
#define FT_LS_HOST      56      /* set "str" to host part of addr      */
#define FT_LS_PATH      57      /* set "str" to route part of addr     */
#define FT_LS_GNAME     58      /* set "str" to group part of addr     */
#define FT_LS_NOTE      59      /* set "str" to comment part of addr   */
#define FT_LS_ADDR      60      /* set "str" to mbox@host              */
#define FT_LS_822ADDR   61      /* set "str" to 822 format addr        */
#define FT_LS_FRIENDLY  62      /* set "str" to "friendly" format addr */
#define FT_LV_HOSTTYPE  63      /* set "value" to addr host type       */
#define FT_LV_INGRPF    64      /* set "value" to addr in-group flag   */
#define FT_LS_UNQUOTE   65      /* remove RFC 2822 quotes from "str"   */
#define FT_LV_NOHOSTF   66     /* set "value" to addr no-host flag */

/* Date Coercion */
#define FT_LOCALDATE    67      /* Coerce date to local timezone */
#define FT_GMTDATE      68      /* Coerce date to gmt            */

/* pre-format processing */
#define FT_PARSEDATE    69      /* parse comp into a date (tws) struct */
#define FT_PARSEADDR    70      /* parse comp into a mailaddr struct   */
#define FT_FORMATADDR   71      /* let external routine format addr    */
#define FT_CONCATADDR   72      /* formataddr w/out duplicate removal  */
#define FT_MYMBOX       73      /* do "mymbox" test on comp            */

/* conditionals & control flow (must be last) */
#define FT_SAVESTR      74      /* save current str reg               */
#define FT_DONE         75      /* stop formatting                    */
#define FT_PAUSE        76      /* pause                              */
#define FT_NOP          77      /* nop                                */
#define FT_GOTO         78      /* (relative) goto                    */
#define FT_IF_S_NULL    79      /* test if "str" null                 */
#define FT_IF_S         80      /* test if "str" non-null             */
#define FT_IF_V_EQ      81      /* test if "value" = literal          */
#define FT_IF_V_NE      82      /* test if "value" != literal         */
#define FT_IF_V_GT      83      /* test if "value" > literal          */
#define FT_IF_MATCH     84      /* test if "str" contains literal     */
#define FT_IF_AMATCH    85      /* test if "str" starts with literal  */
#define FT_S_NULL       86      /* V = 1 if "str" null                */
#define FT_S_NONNULL    87      /* V = 1 if "str" non-null            */
#define FT_V_EQ         88      /* V = 1 if "value" = literal         */
#define FT_V_NE         89      /* V = 1 if "value" != literal        */
#define FT_V_GT         90      /* V = 1 if "value" > literal         */
#define FT_V_MATCH      91      /* V = 1 if "str" contains literal    */
#define FT_V_AMATCH     92      /* V = 1 if "str" starts with literal */

#define IF_FUNCS FT_S_NULL      /* start of "if" functions */



/*
 * crawl_folders.h -- crawl folder hierarchy
 */

#define CRAWL_NUMFOLDERS 100

/* Callbacks return TRUE crawl_folders should crawl the children of `folder'.
 * Callbacks need not duplicate folder, as crawl_folders does not free it. */
typedef boolean (crawl_callback_t)(char *folder, void *baton);

/* Crawl the folder hierarchy rooted at the relative path `dir'.  For each
 * folder, pass `callback' the folder name (as a path relative to the current
 * directory) and `baton'; the callback may direct crawl_folders not to crawl
 * its children; see above. */
void crawl_folders (char *dir, crawl_callback_t *callback, void *baton);
/*
 * scansbr.h -- definitions for scan()
 */

#define	SCNENC	2		/* message just fine, but encrypted(!!) */
#define	SCNMSG	1		/* message just fine                    */
#define	SCNEOF	0		/* empty message                        */
#define	SCNERR	(-1)		/* error message                        */
#define	SCNNUM	(-2)		/* number out of range                  */
#define	SCNFAT	(-3)		/* fatal error                          */

/*
 * default format for `scan' and `inc'
 */

#ifndef	UK
#define	SCANINC_FORMAT	\
"%4(msg)%<(cur)+%| %>%<{replied}-%?{encrypted}E%| %>\
%02(mon{date})/%02(mday{date})%<{date} %|*%>\
%<(mymbox{from})%<{to}To:%14(decode(friendly{to}))%>%>\
%<(zero)%17(decode(friendly{from}))%>  \
%(decode{subject})%<{body}<<%{body}>>%>\n"
#else
#define	SCANINC_FORMAT	\
"%4(msg)%<(cur)+%| %>%<{replied}-%?{encrypted}E%| %>\
%02(mday{date})/%02(mon{date})%<{date} %|*%>\
%<(mymbox{from})%<{to}To:%14(decode(friendly{to}))%>%>\
%<(zero)%17(decode(friendly{from}))%>  \
%(decode{subject})%<{body}<<%{body}>>%>\n"
#endif

#define	WIDTH  78

/*
 * prototypes
 */
int scan (FILE *, int, int, char *, int, int, int, char *, long, int,
          charstring_t *);




/*
 * tws.h
 */

/* If the following is #defined, a timezone given as a numeric-only offset will
   be treated specially if it's in a zone that observes Daylight Saving Time.
   For instance, during DST, a Date: like "Mon, 24 Jul 2000 12:31:44 -0700" will
   be printed as "Mon, 24 Jul 2000 12:31:44 PDT".  Without the code activated by
   the following #define, that'd be incorrectly printed as "...MST". */
#define	ADJUST_NUMERIC_ONLY_TZ_OFFSETS_WRT_DST 1

struct tws {
    int tw_sec;		/* seconds after the minute - [0, 61] */
    int tw_min;		/* minutes after the hour - [0, 59]   */
    int tw_hour;	/* hour since midnight - [0, 23]      */
    int tw_mday;	/* day of the month - [1, 31]         */
    int tw_mon;		/* months since January - [0, 11]     */
    int tw_year;	/* 4 digit year (ie, 1997)            */
    int tw_wday;	/* days since Sunday - [0, 6]         */
    int tw_yday;	/* days since January 1 - [0, 365]    */
    int tw_zone;
    time_t tw_clock;	/* if != 0, corresponding calendar value */
    int tw_flags;
};

#define	TW_NULL	 0x0000

#define	TW_SDAY	 0x0003	/* how day-of-week was determined */
#define	TW_SNIL	 0x0000	/*   not given                    */
#define	TW_SEXP	 0x0001	/*   explicitly given             */
#define	TW_SIMP	 0x0002	/*   implicitly given             */

#define	TW_SZONE 0x0004	/* how timezone was determined    */
#define	TW_SZNIL 0x0000	/*   not given                    */
#define	TW_SZEXP 0x0004	/*   explicitly given             */

#define	TW_DST	 0x0010	/* daylight savings time          */
#define	TW_ZONE	 0x0020	/* use numeric timezones only     */

#define TW_SUCC  0x0040 /* whether parsing was successful */
#define TW_YES   0x0040 /*   yes, found                   */
#define TW_NO    0x0000 /*   no, not found                */

#define	dtwszone(tw) dtimezone (tw->tw_zone, tw->tw_flags)

extern char *tw_dotw[];
extern char *tw_ldotw[];
extern char *tw_moty[];

/*
 * prototypes
 */
char *dtime (time_t *, int);
char *dtimenow (int);
char *dctime (struct tws *);
struct tws *dlocaltimenow (void);
struct tws *dlocaltime (time_t *);
struct tws *dgmtime (time_t *);
char *dasctime (struct tws *, int);
char *dtimezone (int, int);
void twscopy (struct tws *, struct tws *);
int twsort (struct tws *, struct tws *);
time_t dmktime (struct tws *);
void set_dotw (struct tws *);

struct tws *dparsetime (char *);




/*
 * dropsbr.h -- definitions for maildrop-style files
 */

/*
 * A file which is formatted like a maildrop may have a corresponding map
 * file which is an index to the bounds of each message.  The first record
 * of such an map is special, it contains:
 *
 *      d_id    = number of messages in file
 *      d_size	= version number of map
 *      d_start = last message read
 *      d_stop  = size of file
 *
 *  Each record after that contains:
 *
 *      d_id	= BBoard-ID: of message, or similar info
 *      d_size	= size of message in ARPA Internet octets (\n == 2 octets)
 *      d_start	= starting position of message in file
 *      d_stop	= stopping position of message in file
 *
 * Note that d_start/d_stop do NOT include the message delimiters, so
 * programs using the map can simply fseek to d_start and keep reading
 * until the position is at d_stop.
 */

/*
 * various formats for maildrop files
 */
#define OTHER_FORMAT 0
#define MBOX_FORMAT  1
#define MMDF_FORMAT  2

#define	DRVRSN 3

struct drop {
    int   d_id;
    int	  d_size;
    off_t d_start;
    off_t d_stop;
};

/*
 * prototypes
 */
int mbx_open (char *, int, uid_t, gid_t, mode_t);
int mbx_read (FILE *, long, struct drop **, int);
int mbx_write(char *, int, FILE *, int, long, long, off_t, int, int);
int mbx_copy (char *, int, int, int, int, char *, int);
int mbx_size (int, off_t, off_t);
int mbx_close (char *, int);
char *map_name (char *);
int map_read (char *, long, struct drop **, int);
int map_write (char *, int, int, long, off_t, off_t, long, int, int);
int map_chk (char *, int, struct drop *, long, int);





/*
 * popsbr.h -- header for POP client subroutines
 */

int pop_init (char *, char *, char *, char *, char *, int, int, char *);
int pop_fd (char *, int, char *, int);
int pop_stat (int *, int *);
int pop_retr (int, int (*)(char *));
int pop_dele (int);
int pop_noop (void);
int pop_rset (void);
int pop_top (int, int, int (*)(char *));
int pop_quit (void);
int pop_done (void);
int pop_set (int, int, int);
int pop_list (int, int *, int *, int *);




/*
 * mime.h -- definitions for MIME
 */

#define	VRSN_FIELD	"MIME-Version"
#define	VRSN_VALUE	"1.0"
#define	XXX_FIELD_PRF	"Content-"
#define	TYPE_FIELD	"Content-Type"
#define	ENCODING_FIELD	"Content-Transfer-Encoding"
#define	ID_FIELD	"Content-ID"
#define	DESCR_FIELD	"Content-Description"
#define	DISPO_FIELD	"Content-Disposition"
#define	MD5_FIELD	"Content-MD5"
#define	ATTACH_FIELD	"Attach"

#define	isatom(c)   (isascii((unsigned char) c) \
                     && !isspace ((unsigned char) c) \
		     && !iscntrl ((unsigned char) c) && (c) != '(' \
	             && (c) != ')' && (c) != '<'  && (c) != '>' \
	             && (c) != '@' && (c) != ','  && (c) != ';' \
	             && (c) != ':' && (c) != '\\' && (c) != '"' \
	             && (c) != '.' && (c) != '['  && (c) != ']')

/*
 * Test for valid characters used in "token"
 * as defined in RFC2045
 */
#define	istoken(c)  (isascii((unsigned char) c) \
                     && !isspace ((unsigned char) c) \
		     && !iscntrl ((unsigned char) c) && (c) != '(' \
	             && (c) != ')' && (c) != '<'  && (c) != '>' \
	             && (c) != '@' && (c) != ','  && (c) != ';' \
	             && (c) != ':' && (c) != '\\' && (c) != '"' \
	             && (c) != '/' && (c) != '['  && (c) != ']' \
	             && (c) != '?' && (c) != '=')

/*
 * Definitions for RFC 2231 encoding
 */
#define istspecial(c)  ((c) == '(' || (c) == ')' || (c) == '<' || (c) == '>' \
		        || (c) == '@' || (c) == ',' || (c) == ';' \
			|| (c) == ':' || (c) == '\\' || (c) == '"' \
			|| (c) == '/' || (c) == '[' || (c) == ']' \
			|| (c) == '?' || (c) == '=')

#define isparamencode(c)  (!isascii((unsigned char) c) || \
			   iscntrl((unsigned char) c) || istspecial(c) || \
			   (c) == ' ' || (c) == '*' || (c) == '\'' || \
			   (c) == '%')

#define	MAXTEXTPERLN 78
#define	MAXLONGLINE 998
#define	CPERLIN	76
#define	BPERLIN	(CPERLIN / 4)
#define	LPERMSG	632
#define	CPERMSG	(LPERMSG * CPERLIN)




/*
 * mhcachesbr.h -- definitions for manipulating MIME content cache
 */

/*
 * various cache policies
 */

#define CACHE_SWITCHES \
    X("never", 0, CACHE_NEVER) \
    X("private", 0, CACHE_PRIVATE) \
    X("public", 0, CACHE_PUBLIC) \
    X("ask", 0, CACHE_ASK) \

#define X(sw, minchars, id) id,
DEFINE_SWITCH_ENUM(CACHE);
#undef X

#define X(sw, minchars, id) { sw, minchars, id },
DEFINE_SWITCH_ARRAY(CACHE, caches);
#undef X




/*
 * rcvmail.h -- rcvmail hook definitions
 */


#define RCV_MOK	0
#define RCV_MBX	1




/*
 * smtp.h -- definitions for the nmh SMTP Interface
 */

/* length is the length of the string in text[], which is also NUL
 * terminated, so s.text[s.length] should always be 0.
 */
struct smtp {
    int code;
    int length;
    char text[BUFSIZ];
};

/*
 * prototypes
 */
/* int client (); */
int sm_init (char *, char *, char *, int, int, int, int, int, char *, char *, int);
int sm_winit (char *);
int sm_wadr (char *, char *, char *);
int sm_waend (void);
int sm_wtxt (char *, int);
int sm_wtend (void);
int sm_end (int);
char *rp_string (int);

/* The remainder of this file is derived from "mmdf.h" */

/*
 *     Copyright (C) 1979,1980,1981,1982,1983  University of Delaware
 *     Used by permission, May, 1984.
 */

/*
 *     MULTI-CHANNEL MEMO DISTRIBUTION FACILITY  (MMDF)
 *     
 *
 *     Copyright (C) 1979,1980,1981,1982,1983  University of Delaware
 *     
 *     Department of Electrical Engineering
 *     University of Delaware
 *     Newark, Delaware  19711
 *
 *     Phone:  (302) 738-1163
 *     
 *     
 *     This program module was developed as part of the University
 *     of Delaware's Multi-Channel Memo Distribution Facility (MMDF).
 *     
 *     Acquisition, use, and distribution of this module and its listings
 *     are subject restricted to the terms of a license agreement.
 *     Documents describing systems using this module must cite its source.
 *
 *     The above statements must be retained with all copies of this
 *     program and may not be removed without the consent of the
 *     University of Delaware.
 *     
 */

/*                      Reply Codes for MMDF
 *
 *  Based on: "Revised FTP Reply Codes", by Jon Postel & Nancy Neigus Arpanet
 *      RFC 640 / NIC 30843, in the "Arpanet Protocol Handbook", E.  Feinler
 *      and J. Postel (eds.), NIC 7104, Network Information Center, SRI
 *      International:  Menlo Park, CA.  (NTIS AD-A0038901)
 *
 *  Actual values are different, but scheme is same.  Codes must fit into
 *  8-bits (to pass on exit() calls); fields are packed 2-3-3 and interpreted
 *  as octal numbers.
 *
 *  Basic format:
 *
 *      0yz: positive completion; entire action done
 *      1yz: positive intermediate; only part done
 *      2yz: Transient negative completion; may work later
 *      3yz: Permanent negative completion; you lose forever
 *
 *      x0z: syntax
 *      x1z: general; doesn't fit any other category
 *      x2z: connections; truly transfer-related
 *      x3z: user/authentication/account
 *      x4x: mail
 *      x5z: file system
 *
 *      3-bit z field is unique to the reply.  In the following,
 *      the RP_xVAL defines are available for masking to obtain a field.
 */

/*
 * FIELD DEFINITIONS & BASIC VALUES
 */

/* FIELD 1:  Basic degree of success (2-bits) */

#define RP_BTYP '\200'      /* good vs. bad; on => bad            */
#define RP_BVAL '\300'      /* basic degree of success            */

#define RP_BOK  '\000'      /* went fine; all done                */
#define RP_BPOK '\100'      /* only the first part got done       */
#define RP_BTNO '\200'      /* temporary failure; try later       */
#define RP_BNO  '\300'      /* not now, nor never; you lose       */

/* FIELD 2:  Basic domain of discourse (3-bits) */

#define RP_CVAL '\070'      /* basic category (domain) of reply   */

#define RP_CSYN '\000'      /* purely a matter of form            */
#define RP_CGEN '\010'      /* couldn't find anywhere else for it */
#define RP_CCON '\020'      /* data-transfer-related issue        */
#define RP_CUSR '\030'      /* pertaining to the user             */
#define RP_CMAI '\040'      /* specific to mail semantics         */
#define RP_CFIL '\050'      /* file system                        */
#define RP_CLIO '\060'      /* local i/o system                   */

/* FIELD 3:  Specific value for this reply (3-bits) */

#define RP_SVAL '\007'      /* specific value of reply            */


/*
 * SPECIFIC SUCCESS VALUES
 */

/*
 * Complete Success
 */

/* done (e.g., w/transaction) */
#define RP_DONE (RP_BOK | RP_CGEN | '\000')

/* general-purpose OK */
#define RP_OK   (RP_BOK | RP_CGEN | '\001')

/* message is accepted (w/text) */
#define RP_MOK  (RP_BOK | RP_CMAI | '\000')


/*
 * Partial Success
 */

/* you are the requestor */
#define RP_MAST (RP_BPOK| RP_CGEN | '\000')

/* you are the requestee */
#define RP_SLAV (RP_BPOK| RP_CGEN | '\001')

/* message address is accepted */
#define RP_AOK  (RP_BPOK| RP_CMAI | '\000')


/*
 * SPECIFIC FALURE VALUES
 */

/*
 * Partial Failure
 */

/* not now; maybe later */
#define RP_AGN  (RP_BTNO | RP_CGEN | '\000')

/* timeout */
#define RP_TIME (RP_BTNO | RP_CGEN | '\001')

/* no-op; nothing done, this time */
#define RP_NOOP (RP_BTNO | RP_CGEN | '\002')

/* encountered an end of file */
#define RP_EOF  (RP_BTNO | RP_CGEN | '\003')

/* channel went bad */
#define RP_NET  (RP_BTNO | RP_CCON | '\000')

/* foreign host screwed up */
#define RP_BHST (RP_BTNO | RP_CCON | '\001')

/* host went away */
#define RP_DHST (RP_BTNO | RP_CCON | '\002')

/* general net i/o problem */
#define RP_NIO  (RP_BTNO | RP_CCON | '\004')

/* error reading/writing file */
#define RP_FIO  (RP_BTNO | RP_CFIL | '\000')

/* unable to create file */
#define RP_FCRT (RP_BTNO | RP_CFIL | '\001')

/* unable to open file */
#define RP_FOPN (RP_BTNO | RP_CFIL | '\002')

/* general local i/o problem */
#define RP_LIO  (RP_BTNO | RP_CLIO | '\000')

/* resource currently locked */
#define RP_LOCK (RP_BTNO | RP_CLIO | '\001')


/*
 * Complete Failure
 */

/* bad mechanism/path; try alternate? */
#define RP_MECH (RP_BNO | RP_CGEN | '\000')

/* general-purpose NO */
#define RP_NO   (RP_BNO | RP_CGEN | '\001')

/* general prototocol error */
#define RP_PROT (RP_BNO | RP_CCON | '\000')

/* bad reply code (PERMANENT ERROR) */
#define RP_RPLY (RP_BNO | RP_CCON | '\001')

/* couldn't deliver */
#define RP_NDEL (RP_BNO | RP_CMAI | '\000')

/* couldn't parse the request */
#define RP_HUH  (RP_BNO | RP_CSYN | '\000')

/* no such command defined */
#define RP_NCMD (RP_BNO | RP_CSYN | '\001')

/* bad parameter */
#define RP_PARM (RP_BNO | RP_CSYN | '\002')

/* command not implemented */
#define RP_UCMD (RP_BNO | RP_CSYN | '\003')

/* unknown user */
#define RP_USER (RP_BNO | RP_CUSR | '\000')


/*
 * Macros to access reply info
 */

/* get the entire return value */
#define rp_gval(val)    ((signed char) (val))


/*
 * The next three give the field's bits, within the whole value
 */

/* get the basic part of return value */
#define rp_gbval(val)   (rp_gval (val) & RP_BVAL)

/* get the domain part of value */
#define rp_gcval(val)   (rp_gval (val) & RP_CVAL)

/* get the specific part of value */
#define rp_gsval(val)   (rp_gval (val) & RP_SVAL)


/*
 * The next three give the numeric value withing the field
 */

/* get the basic part right-shifted */
#define rp_gbbit(val)   ((rp_gval (val) >> 6) & 03)

/* get the domain part right-shifted */
#define rp_gcbit(val)   ((rp_gval (val) >> 3) & 07)

/* get the specific part right-shifted */
#define rp_gsbit(val)   (rp_gval (val) & 07)


/*
 * MACHINE DEPENDENCY
 *
 * The following treat the value as strictly numeric.
 * It relies on the negative values being numerically
 * negative.
 */

/* is return value positive? */
#define rp_isgood(val)  (rp_gval (val) >= 0)

/* is return value negative? */
#define rp_isbad(val)   (rp_gval (val) < 0)




/*
 * picksbr.h -- definitions for picksbr.c
 */

/*
 * prototypes
 */
int pcompile (char **, char *);
int pmatches (FILE *, int, long, long, int);
