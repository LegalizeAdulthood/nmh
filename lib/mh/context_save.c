
/*
 * context_save.c -- write out the updated context file
 *
 * This code is Copyright (c) 2002, by the authors of nmh.  See the
 * COPYRIGHT file in the root directory of the nmh distribution for
 * complete copyright information.
 */

#include <libmh.h>

#include <signal.h>

/*
 * static prototypes
 */
static int m_chkids(void);


void
context_save (void)
{
    int action;
    register struct node *np;
    FILE *out;
    sigset_t set, oset;
    int failed_to_lock = 0;
    
    /* No context in use -- silently ignore any changes! */
    if (!ctxpath)
       return;

    if (!(ctxflags & CTXMOD))
	return;
    ctxflags &= ~CTXMOD;

    if ((action = m_chkids ()) > 0)
	return;			/* child did it for us */

    /* block a few signals */
    sigemptyset (&set);
    sigaddset (&set, SIGHUP);
    sigaddset (&set, SIGINT);
    sigaddset (&set, SIGQUIT);
    sigaddset (&set, SIGTERM);
    sigprocmask (SIG_BLOCK, &set, &oset);

    if (!(out = lkfopendata (ctxpath, "w", &failed_to_lock))) {
	if (failed_to_lock) {
	    adios (ctxpath, "failed to lock");
	} else {
	    adios (ctxpath, "unable to write");
	}
    }
    for (np = m_defs; np; np = np->n_next)
	if (np->n_context)
	    fprintf (out, "%s: %s\n", np->n_name, np->n_field);
    lkfclosedata (out, ctxpath);

    sigprocmask (SIG_SETMASK, &oset, &set); /* reset the signal mask */

    if (action == 0)
	/* This must be _exit(), not exit(), because the child didn't
	   call unregister_for_removal() in m_chkids(). */
	_exit (0);		/* we are child, time to die */
}

/*
 * This hack brought to you so we can handle set[ug]id MH programs.
 * If we return -1, then no fork is made, we update .mh_profile
 * normally, and return to the caller normally.  If we return 0,
 * then the child is executing, .mh_profile is modified after
 * we set our [ug]ids to the norm.  If we return > 0, then the
 * parent is executed and .mh_profile has already be modified.
 * We can just return to the caller immediately.
 */

static int
m_chkids (void)
{
    int i;
    pid_t pid;

    if (getuid () == geteuid ())
	return (-1);

    for (i = 0; (pid = fork ()) == -1 && i < 5; i++)
	sleep (5);

    switch (pid) {
	case -1:
	    break;

	case 0:
	    /* It's not necessary to call unregister_for_removal(0)
               because the child calls _exit() in context_save(). */
	    break;

	default:
	    pidwait (pid, -1);
	    break;
    }

    return pid;
}
