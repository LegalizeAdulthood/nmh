/*
 * mhlogin.c -- login to external (OAuth) services
 *
 * This code is Copyright (c) 2014, by the authors of nmh.  See the
 * COPYRIGHT file in the root directory of the nmh distribution for
 * complete copyright information.
 */

#include <stdio.h>
#include <string.h>

#include <h/mh.h>
#include <h/oauth.h>

#define MHLOGIN_SWITCHES \
    X("oauth", 1, OAUTHSW) \
    X("snoop", 1, SNOOPSW) \
    X("help", 1, HELPSW) \
    X("version", 1, VERSIONSW) \

#define X(sw, minchars, id) id,
DEFINE_SWITCH_ENUM(MHLOGIN);
#undef X

#define X(sw, minchars, id) { sw, minchars, id },
DEFINE_SWITCH_ARRAY(MHLOGIN, switches);
#undef X

#ifdef OAUTH_SUPPORT
/* XXX copied from install-mh.c */
static char *
geta (void)
{
    char *cp;
    static char line[BUFSIZ];

    if (fgets(line, sizeof(line), stdin) == NULL)
	done (1);
    if ((cp = strchr(line, '\n')))
	*cp = 0;
    return line;
}

static int
do_login(const char *svc, int snoop)
{
    char *fn, *code;
    mh_oauth_ctx *ctx;
    mh_oauth_cred *cred;
    FILE *cred_file;
    int failed_to_lock = 0;
    const char *url;

    if (svc == NULL) {
        adios(NULL, "only support -oauth gmail");
    }

    if (!mh_oauth_new(&ctx, svc)) {
        adios(NULL, mh_oauth_get_err_string(ctx));
    }

    if (snoop) {
        mh_oauth_log_to(stderr, ctx);
    }

    fn = getcpy(mh_oauth_cred_fn(ctx));

    if ((url = mh_oauth_get_authorize_url(ctx)) == NULL) {
      adios(NULL, mh_oauth_get_err_string(ctx));
    }

    printf("Load the following URL in your browser and authorize nmh"
           " to access %s:\n"
           "\n%s\n\n"
           "Enter the authorization code: ",
           mh_oauth_svc_display_name(ctx), url);
    fflush(stdout);
    code = geta();

    while ((cred = mh_oauth_authorize(code, ctx)) == NULL
           && mh_oauth_get_err_code(ctx) == MH_OAUTH_BAD_GRANT) {
      printf("Code rejected; try again? ");
      fflush(stdout);
      code = geta();
    }
    if (cred == NULL) {
      advise(NULL, "error exchanging code for OAuth2 token");
      adios(NULL, mh_oauth_get_err_string(ctx));
    }

    cred_file = lkfopendata(fn, "w", &failed_to_lock);
    if (cred_file == NULL || failed_to_lock) {
      adios(fn, "oops");
    }
    if (!mh_oauth_cred_save(cred_file, cred)) {
      adios(NULL, mh_oauth_get_err_string(ctx));
    }
    if (lkfclosedata(cred_file, fn) != 0) {
      adios (fn, "oops");
    }

    mh_oauth_cred_free(cred);
    mh_oauth_free(ctx);

    return 0;
}
#endif

int
main(int argc, char **argv)
{
    char *cp, **argp, **arguments;
    char *svc = NULL;
    int snoop = 0;

    if (nmh_init(argv[0], 1)) { return 1; }

    arguments = getarguments (invo_name, argc, argv, 1);
    argp = arguments;

    while ((cp = *argp++)) {
	if (*cp == '-') {
            char help[BUFSIZ];
	    switch (smatch (++cp, switches)) {
	    case AMBIGSW:
		ambigsw (cp, switches);
		done (1);
	    case UNKWNSW:
		adios (NULL, "-%s unknown", cp);

	    case HELPSW:
		snprintf(help, sizeof(help), "%s -oauth gmail [switches]",
                         invo_name);
		print_help (help, switches, 1);
		done (0);
	    case VERSIONSW:
		print_version(invo_name);
		done (0);

            case OAUTHSW:
		    if (!(cp = *argp++) || *cp == '-')
			adios (NULL, "missing argument to %s", argp[-2]);
                    svc = cp;
                continue;

            case SNOOPSW:
                snoop++;
                continue;
	    }
	}
        adios(NULL, "extraneous arguments");
    }

#ifdef OAUTH_SUPPORT
    return do_login(svc, snoop);
#else
    NMH_UNUSED(svc);
    NMH_UNUSED(snoop);
    adios(NULL, "not built with OAuth support");
    return 1;
#endif
}