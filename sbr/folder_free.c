
/*
 * folder_free.c -- free a folder/message structure
 *
 * This code is Copyright (c) 2002, by the authors of nmh.  See the
 * COPYRIGHT file in the root directory of the nmh distribution for
 * complete copyright information.
 */

#include <h/mh.h>
#include <h/utils.h>


/*
 * To prevent leaks of struct msgs allocated by folder_read():  it always
 * calls register_folder_for_cleanup(), which saves the address of the
 * allocated struct msgs in a list of folders_node's.  At program exit,
 * remove_registered_folders_atexit () is called to free each of them.
 * To allow programs, and (notably) m_draft(), to call folder_free (),
 * it calls unregister_folder_for_cleanup() to remove the pointer from
 * the list, and thereby prevent double deletion.
 */
struct folders_node {
    struct msgs *folder;
    struct folders_node *next;
};

static struct folders_node *folders_for_cleanup;

static void unregister_folder_for_cleanup (struct msgs *);


void
folder_free (struct msgs *mp)
{
    size_t i;
    bvector_t *v;

    if (!mp)
	return;

    mh_xfree(mp->foldpath);

    /* free the sequence names */
    for (i = 0; i < svector_size (mp->msgattrs); i++)
	free (svector_at (mp->msgattrs, i));
    svector_free (mp->msgattrs);

    for (i = 0, v = mp->msgstats; i < mp->num_msgstats; ++i, ++v) {
	bvector_free (*v);
    }
    free (mp->msgstats);

    /* Close/free the sequence file if it is open */

    if (mp->seqhandle)
    	lkfclosedata (mp->seqhandle, mp->seqname);

    mh_xfree(mp->seqname);

    bvector_free (mp->attrstats);
    unregister_folder_for_cleanup (mp);
    free (mp);			/* free main folder structure */
}


/*
 * Store a struct msgs * so that it can be free'd at exit.
 */
void
register_folder_for_cleanup(struct msgs *mp) {
    struct folders_node *node =
        (struct folders_node *) mh_xmalloc (sizeof (struct folders_node));

    node->folder = mp;
    /* Push new node on front of list. */
    node->next = folders_for_cleanup;
    folders_for_cleanup = node;
}


/*
 * Free a specific saved msg struct's node from the saved msg structs.
 */
static void
unregister_folder_for_cleanup (struct msgs *mp) {
    struct folders_node *node = folders_for_cleanup;

    /* Look in first node for match.  It's most likely, and avoids
       another conditional inside the loop below. */
    if (node  &&  node->folder == mp) {
        folders_for_cleanup = node->next;
        mh_xfree (node);
    } else {
        struct folders_node *prev = folders_for_cleanup;

        /* Walk list, and free the first matching node  The struct msgs
           will be free'd by free_folder(), which called this function. */
        while (node) {
            if (node->folder == mp) {
                prev->next = node->next;
                mh_xfree (node);
                break;
            } else {
                prev = node;
                node = node->next;
            }
        }
    }
}


/*
 * Remove all of the saved msg structs.
 */
void
remove_registered_folders_atexit () {
    struct folders_node *node = folders_for_cleanup;

    /* Each struct msgs will be free'd by free_folder() and each list
       node will be free'd by unregister_folder_for_cleanup(). */
    while (node) {
        struct folders_node *next = node->next;

        folder_free (node->folder);
        node = next;
    }
}
