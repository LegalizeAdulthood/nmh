
/*
 * mhshow.c -- display the contents of MIME messages
 *
 * This code is Copyright (c) 2002, by the authors of nmh.  See the
 * COPYRIGHT file in the root directory of the nmh distribution for
 * complete copyright information.
 */

#include <h/mh.h>
#include <fcntl.h>
#include <h/signals.h>
#include <h/md5.h>
#include <h/mts.h>
#include <h/tws.h>
#include <h/mime.h>
#include <h/mhparse.h>
#include <h/mhcachesbr.h>
#include <h/utils.h>

#define MHSHOW_SWITCHES \
    X("check", 0, CHECKSW) \
    X("nocheck", 0, NCHECKSW) \
    X("verbose", 0, VERBSW) \
    X("noverbose", 0, NVERBSW) \
    X("concat", 0, CONCATSW) \
    X("noconcat", 0, NCONCATSW) \
    X("textonly", 0, TEXTONLYSW) \
    X("notextonly", 0, NTEXTONLYSW) \
    X("inlineonly", 0, INLINESW) \
    X("noinlineonly", 0, NINLINESW) \
    X("file file", 0, FILESW) \
    X("form formfile", 0, FORMSW) \
    X("header", 0, HEADSW) \
    X("noheader", 0, NHEADSW) \
    X("headerform formfile", 0, HEADFORMSW) \
    X("markform formfile", 0, MARKFORMSW) \
    X("part number", 0, PARTSW) \
    X("type content", 0, TYPESW) \
    X("prefer content", 0, PREFERSW) \
    X("rcache policy", 0, RCACHESW) \
    X("wcache policy", 0, WCACHESW) \
    X("version", 0, VERSIONSW) \
    X("help", 0, HELPSW) \
    /*					\
     * switches for moreproc/mhlproc	\
     */					\
    X("moreproc program", -4, PROGSW) \
    X("nomoreproc", -3, NPROGSW) \
    X("length lines", -4, LENSW) \
    X("width columns", -4, WIDTHSW) \
    /*				\
     * switches for debugging	\
     */				\
    X("debug", -5, DEBUGSW) \

#define X(sw, minchars, id) id,
DEFINE_SWITCH_ENUM(MHSHOW);
#undef X

#define X(sw, minchars, id) { sw, minchars, id },
DEFINE_SWITCH_ARRAY(MHSHOW, switches);
#undef X


/* mhcachesbr.c */
extern int rcachesw;
extern int wcachesw;
extern char *cache_public;
extern char *cache_private;

/* mhshowsbr.c */
extern char *progsw;
extern int nomore;	/* flags for moreproc/header display */
extern char *formsw;
extern char *folder;
extern char *headerform;
extern char *markerform;
extern int headersw;

/* mhmisc.c */
extern int npart;
extern int ntype;
extern char *parts[NPARTS + 1];
extern char *types[NTYPES + 1];
extern int userrs;

/* mhparse.c */
extern char *preferred_types[];
extern  char *preferred_subtypes[];
extern int npreferred;

int debugsw = 0;
int verbosw = 0;

#define	quitser	pipeser

/* mhparse.c */
CT parse_mime (char *);

/* mhmisc.c */
int part_ok (CT);
int type_ok (CT, int);
void flush_errors (void);

/* mhfree.c */
extern CT *cts;
void freects_done (int) NORETURN;

/*
 * static prototypes
 */
static void pipeser (int);


int
main (int argc, char **argv)
{
    int msgnum, *icachesw, concatsw = -1, textonly = -1, inlineonly = -1;
    char *cp, *file = NULL;
    char *maildir, buf[100], **argp;
    char **arguments;
    struct msgs_array msgs = { 0, 0, NULL };
    struct msgs *mp = NULL;
    CT ct, *ctp;
    FILE *fp;

    if (nmh_init(argv[0], 1)) { return 1; }

    done=freects_done;

    arguments = getarguments (invo_name, argc, argv, 1);
    argp = arguments;

    /*
     * Parse arguments
     */
    while ((cp = *argp++)) {
	if (*cp == '-') {
	    switch (smatch (++cp, switches)) {
	    case AMBIGSW:
		ambigsw (cp, switches);
		done (1);
	    case UNKWNSW:
		adios (NULL, "-%s unknown", cp);

	    case HELPSW:
		snprintf (buf, sizeof(buf), "%s [+folder] [msgs] [switches]",
			invo_name);
		print_help (buf, switches, 1);
		done (0);
	    case VERSIONSW:
		print_version(invo_name);
		done (0);

	    case RCACHESW:
		icachesw = &rcachesw;
		goto do_cache;
	    case WCACHESW:
		icachesw = &wcachesw;
do_cache:
		if (!(cp = *argp++) || *cp == '-')
		    adios (NULL, "missing argument to %s", argp[-2]);
		switch (*icachesw = smatch (cp, caches)) {
		case AMBIGSW:
		    ambigsw (cp, caches);
		    done (1);
		case UNKWNSW:
		    adios (NULL, "%s unknown", cp);
		default:
		    break;
		}
		continue;

	    case CHECKSW:
		checksw++;
		continue;
	    case NCHECKSW:
		checksw = 0;
		continue;

	    case CONCATSW:
		concatsw = 1;
		continue;
	    case NCONCATSW:
		concatsw = 0;
		continue;
	    case TEXTONLYSW:
		textonly = 1;
		continue;
	    case NTEXTONLYSW:
		textonly = 0;
		continue;
	    case INLINESW:
		inlineonly = 1;
		continue;
	    case NINLINESW:
		inlineonly = 0;
		continue;

	    case PARTSW:
		if (!(cp = *argp++) || *cp == '-')
		    adios (NULL, "missing argument to %s", argp[-2]);
		if (npart >= NPARTS)
		    adios (NULL, "too many parts (starting with %s), %d max",
			   cp, NPARTS);
		parts[npart++] = cp;
		continue;

	    case TYPESW:
		if (!(cp = *argp++) || *cp == '-')
		    adios (NULL, "missing argument to %s", argp[-2]);
		if (ntype >= NTYPES)
		    adios (NULL, "too many types (starting with %s), %d max",
			   cp, NTYPES);
		types[ntype++] = cp;
		continue;

	    case PREFERSW:
		if (!(cp = *argp++) || *cp == '-')
		    adios (NULL, "missing argument to %s", argp[-2]);
		if (npreferred >= NPREFS)
		    adios (NULL, "too many preferred types (starting with %s), %d max",
			   cp, NPREFS);
		preferred_types[npreferred] = cp;
		cp = strchr(cp, '/');
		if (cp) *cp++ = '\0';
		preferred_subtypes[npreferred++] = cp;
		continue;

	    case FILESW:
		if (!(cp = *argp++) || (*cp == '-' && cp[1]))
		    adios (NULL, "missing argument to %s", argp[-2]);
		file = *cp == '-' ? cp : path (cp, TFILE);
		continue;

	    case FORMSW:
		if (!(cp = *argp++) || *cp == '-')
		    adios (NULL, "missing argument to %s", argp[-2]);
                mh_xfree(formsw);
		formsw = getcpy (etcpath (cp));
		continue;

	    case HEADSW:
		headersw = 1;
		continue;
	    case NHEADSW:
		headersw = 0;
		continue;

	    case HEADFORMSW:
		if (!(headerform = *argp++) || *headerform == '-')
		    adios (NULL, "missing argument to %s", argp[-2]);
		continue;

	    case MARKFORMSW:
		if (!(markerform = *argp++) || *markerform == '-')
		    adios (NULL, "missing argument to %s", argp[-2]);
		continue;

	    /*
	     * Switches for moreproc/mhlproc
	     */
	    case PROGSW:
		if (!(progsw = *argp++) || *progsw == '-')
		    adios (NULL, "missing argument to %s", argp[-2]);
		continue;
	    case NPROGSW:
		nomore++;
		continue;

	    case LENSW:
	    case WIDTHSW:
		if (!(cp = *argp++) || *cp == '-')
		    adios (NULL, "missing argument to %s", argp[-2]);
		continue;

	    case VERBSW:
		verbosw = 1;
		continue;
	    case NVERBSW:
		verbosw = 0;
		continue;
	    case DEBUGSW:
		debugsw = 1;
		continue;
	    }
	}
	if (*cp == '+' || *cp == '@') {
	    if (folder)
		adios (NULL, "only one folder at a time!");
	    else
		folder = pluspath (cp);
	} else
		app_msgarg(&msgs, cp);
    }

    /* null terminate the list of acceptable parts/types */
    parts[npart] = NULL;
    types[ntype] = NULL;

    /*
     * If we had any specific parts or types specified, turn off text only
     * content.
     */

    if (npart > 0 || ntype > 0) {
	if (textonly == -1)
	    textonly = 0;
	if (inlineonly == -1)
	    inlineonly = 0;
    }

    /*
     * Check if we've specified an additional profile
     */
    if ((cp = getenv ("MHSHOW"))) {
	if ((fp = fopen (cp, "r"))) {
	    readconfig ((struct node **) 0, fp, cp, 0);
	    fclose (fp);
	} else {
	    admonish ("", "unable to read $MHSHOW profile (%s)", cp);
	}
    }

    /*
     * Read the standard profile setup
     */
    if ((fp = fopen (cp = etcpath ("mhn.defaults"), "r"))) {
	readconfig ((struct node **) 0, fp, cp, 0);
	fclose (fp);
    }

    /* Check for public cache location */
    if ((cache_public = context_find (nmhcache)) && *cache_public != '/')
	cache_public = NULL;

    /* Check for private cache location */
    if (!(cache_private = context_find (nmhprivcache)))
	cache_private = ".cache";
    cache_private = getcpy (m_maildir (cache_private));

    if (!context_find ("path"))
	free (path ("./", TFOLDER));

    if (file && msgs.size)
	adios (NULL, "cannot specify msg and file at same time!");

    /*
     * check if message is coming from file
     */
    if (file) {
	cts = mh_xcalloc(2, sizeof *cts);
	ctp = cts;

	if ((ct = parse_mime (file)))
	    *ctp++ = ct;

	headersw = 0;
    } else {
	/*
	 * message(s) are coming from a folder
	 */
	if (!msgs.size)
	    app_msgarg(&msgs, "cur");
	if (!folder)
	    folder = getfolder (1);
	maildir = m_maildir (folder);

	if (chdir (maildir) == NOTOK)
	    adios (maildir, "unable to change directory to");

	/* read folder and create message structure */
	if (!(mp = folder_read (folder, 1)))
	    adios (NULL, "unable to read folder %s", folder);

	/* check for empty folder */
	if (mp->nummsg == 0)
	    adios (NULL, "no messages in %s", folder);

	/* parse all the message ranges/sequences and set SELECTED */
	for (msgnum = 0; msgnum < msgs.size; msgnum++)
	    if (!m_convert (mp, msgs.msgs[msgnum]))
		done (1);

	/*
	 * Set the SELECT_UNSEEN bit for all the SELECTED messages,
	 * since we will use that as a tag to know which messages
	 * to remove from the "unseen" sequence.
	 */
	for (msgnum = mp->lowsel; msgnum <= mp->hghsel; msgnum++)
	    if (is_selected(mp, msgnum))
		set_unseen (mp, msgnum);

	seq_setprev (mp);	/* set the Previous-Sequence */
	seq_setunseen (mp, 1);	/* unset the Unseen-Sequence */

	cts = mh_xcalloc(mp->numsel + 1, sizeof *cts);
	ctp = cts;

	/*
	 * Parse all the SELECTED messages.
	 */
	for (msgnum = mp->lowsel; msgnum <= mp->hghsel; msgnum++) {
	    if (is_selected(mp, msgnum)) {
		char *msgnam;

		msgnam = m_name (msgnum);
		if ((ct = parse_mime (msgnam)))
		    *ctp++ = ct;
	    }
	}
    }

    if (!*cts)
	done (1);

    userrs = 1;
    SIGNAL (SIGQUIT, quitser);
    SIGNAL (SIGPIPE, pipeser);

    /*
     * Get the associated umask for the relevant contents.
     */
    for (ctp = cts; *ctp; ctp++) {
	struct stat st;

	ct = *ctp;
	if (type_ok (ct, 1) && !ct->c_umask) {
	    if (stat (ct->c_file, &st) != NOTOK)
		ct->c_umask = ~(st.st_mode & 0777);
	    else
		ct->c_umask = ~m_gmprot();
	}
    }

    /* If reading from a folder, do some updating */
    if (mp) {
	context_replace (pfolder, folder);/* update current folder  */
	seq_setcur (mp, mp->hghsel);	  /* update current message */
	seq_save (mp);			  /* synchronize sequences  */
	context_save ();		  /* save the context file  */
    }

    if (concatsw)
	m_popen(moreproc, 0);

    /*
     * Show the message content
     */
    show_all_messages (cts, concatsw, textonly, inlineonly);

    /* Now free all the structures for the content */
    for (ctp = cts; *ctp; ctp++)
	free_content (*ctp);

    free(cts);
    cts = NULL;

    if (concatsw)
	m_pclose();

    done (0);
    return 1;
}


static void
pipeser (int i)
{
    if (i == SIGQUIT) {
	fflush (stdout);
	fprintf (stderr, "\n");
	fflush (stderr);
    }

    done (1);
    /* NOTREACHED */
}
