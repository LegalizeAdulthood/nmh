
/*
 * popsbr.h -- header for POP client subroutines
 */

/* TLS flags */
#define P_INITTLS 0x01
#define P_NOVERIFY 0x02
int pop_init (char *, char *, char *, char *, int, int, char *, int,
	      const char *);
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
