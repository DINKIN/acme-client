/*	$Id$ */
/*
 * Copyright (c) 2016 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHORS DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef EXTERN_H
#define EXTERN_H

/*
 * Requests to accounts (signing) infrastructure.
 */
enum	acctop {
	ACCT_STOP,
	ACCT_SIGN,
	ACCT_THUMBPRINT,
	ACCT__MAX
};

/*
 * Our components.
 * Each one of these is in a separated, isolated process.
 */
enum	comp {
	COMP_NET, /* network-facing (to ACME) */
	COMP_KEY, /* handles domain keys */
	COMP_CERT, /* handles domain certificates */
	COMP_ACCOUNT, /* handles account key */
	COMP_CHALLENGE, /* handles challenges */
	COMP__MAX
};

/*
 * Inter-process communication labels.
 * This is purely for looking at debugging.
 */
enum	comm {
	COMM_REQ,
	COMM_THUMB,
	COMM_CERT,
	COMM_PAY,
	COMM_NONCE,
	COMM_TOK,
	COMM_CHNG,
	COMM_CHNG_ACK,
	COMM_CHNG_FIN,
	COMM_ACCT,
	COMM_CSR,
	COMM__MAX
};

/*
 * This contains the URI and token of an ACME-issued challenge.
 * A challenge consists of a token, which we must present on the
 * (presumably!) local machine to an ACME connection; and a URI, to
 * which we must connect to verify the token.
 */
struct challenge {
	char		*uri; /* uri on ACME server */
	char		*token; /* token we must offer */
};

/*
 * This consists of the services offered by the CA.
 * They must all be filled in.
 */
struct	capaths {
	char		*newauthz; /* new authorisation */
	char		*newcert; 
	char		*newreg; /* new acme account */
	char		*revokecert;
};

struct	json;

__BEGIN_DECLS

/*
 * Start with our components.
 * These are all isolated and talk to each other using sockets.
 */
int		 certproc(int, const char *);
int		 netproc(int, int, int, int, int, 
			const char *, const char **, size_t);
int		 acctproc(int, const char *, int);
int		 keyproc(int, const char *, 
			const char *, const char **, size_t);
int		 chngproc(int, const char *);

/*
 * Warning and logging functions.
 * They should be used instead of err.h because they print the process
 * component and pid.
 */
void		 dowarnx(const char *, ...);
void		 dowarn(const char *, ...);
void		 doerr(const char *, ...);
void		 dodbg(const char *, ...);
const char 	*compname(enum comp);

/*
 * Read and write things from the wire.
 * The readers behave differently with respect to EOF.
 */
long		 readop(int, enum comm);
char		*readbuf(int, enum comm, size_t *);
char		*readstr(int, enum comm);
int		 writebuf(int, enum comm, const void *, size_t);
int		 writestr(int, enum comm, const char *);
int		 writeop(int, enum comm, long);

int		 checkexit(pid_t, enum comp);

/*
 * Base64 and URL encoding.
 * Returns a buffer or NULL on allocation error.
 */
size_t		 base64buf(char *, const char *, size_t);
size_t		 base64len(size_t);
char		*base64buf_url(const char *, size_t);

/*
 * JSON parsing routines.
 * Keep this all in on place, though it's only used by one file.
 */
struct json	*json_alloc(void);
void		 json_reset(struct json *);
void		 json_free(struct json *);
size_t		 jsonbody(void *, size_t, size_t, void *);
int		 json_parse_response(struct json *);
void		 json_free_challenge(struct challenge *);
int		 json_parse_challenge(struct json *, struct challenge *);
void		 json_free_capaths(struct capaths *);
int		 json_parse_capaths(struct json *, struct capaths *);

/*
 * Should we print debugging messages?
 */
int		 verbose;

/*
 * What component is the process within (COMP__MAX for none)?
 */
enum comp	 proccomp;

__END_DECLS

#endif /* EXTERN_H */
