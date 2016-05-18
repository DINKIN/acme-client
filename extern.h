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

#define	PATH_VAR_EMPTY "/var/empty"

#define	CERT_PEM "cert.pem"
#define	CERT_BAK "cert.pem~"
#define	CHAIN_PEM "chain.pem"
#define	CHAIN_BAK "chain.pem~"
#define	FCHAIN_PEM "fullchain.pem"
#define	FCHAIN_BAK "fullchain.pem~"


/*
 * Requests to and from acctproc.
 */
enum	acctop {
	ACCT_STOP = 0,
	ACCT_READY,
	ACCT_SIGN,
	ACCT_THUMBPRINT,
	ACCT__MAX
};

/*
 * Requests to and from chngproc.
 */
enum	chngop {
	CHNG_STOP = 0,
	CHNG_SYN,
	CHNG_ACK,
	CHNG__MAX
};

/*
 * Requests to keyproc.
 */
enum	keyop {
	KEY_STOP = 0,
	KEY_READY,
	KEY__MAX
};

/*
 * Requests to certproc.
 */
enum	certop {
	CERT_STOP = 0,
	CERT_REVOKE,
	CERT_UPDATE,
	CERT__MAX
};

/*
 * Requests to fileproc.
 */
enum	fileop {
	FILE_STOP = 0,
	FILE_REMOVE,
	FILE_CREATE,
	FILE__MAX
};

/*
 * Requests to dnsproc.
 */
enum	dnsop {
	DNS_STOP = 0,
	DNS_LOOKUP,
	DNS__MAX
};

enum	revokeop {
	REVOKE_STOP = 0,
	REVOKE_CHECK,
	REVOKE_EXP,
	REVOKE_OK,
	REVOKE__MAX
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
	COMP_FILE, /* handles writing certs */
	COMP_DNS, /* handles DNS lookups */
	COMP_REVOKE, /* checks X509 expiration */
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
	COMM_CHNG_OP,
	COMM_CHNG_ACK,
	COMM_ACCT,
	COMM_ACCT_STAT,
	COMM_CSR,
	COMM_CSR_OP,
	COMM_ISSUER,
	COMM_CHAIN,
	COMM_CHAIN_OP,
	COMM_DNS,
	COMM_DNSQ,
	COMM_DNSA,
	COMM_DNSLEN,
	COMM_KEY_STAT,
	COMM_REVOKE_OP,
	COMM_REVOKE_CHECK,
	COMM_REVOKE_RESP,
	COMM__MAX
};

/*
 * This contains the URI and token of an ACME-issued challenge.
 * A challenge consists of a token, which we must present on the
 * (presumably!) local machine to an ACME connection; and a URI, to
 * which we must connect to verify the token.
 */
struct 	chng {
	char		*uri; /* uri on ACME server */
	char		*token; /* token we must offer */
	size_t		 retry; /* how many times have we tried */
	int		 status; /* challenge accepted? */
};

/*
 * This consists of the services offered by the CA.
 * They must all be filled in.
 */
struct	capaths {
	char		*newauthz; /* new authorisation */
	char		*newcert;  /* sign certificate */
	char		*newreg; /* new acme account */
	char		*revokecert; /* revoke certificate */
};

struct	json;

__BEGIN_DECLS

/*
 * Start with our components.
 * These are all isolated and talk to each other using sockets.
 */
int		 acctproc(int, const char *, int, uid_t, gid_t);
int		 certproc(int, int, uid_t, gid_t);
int		 chngproc(int, const char *, int);
int		 dnsproc(int, uid_t, gid_t);
int		 revokeproc(int, const char *, uid_t, gid_t, int, int);
int		 fileproc(int, const char *);
int		 keyproc(int, const char *, uid_t, gid_t, 
			const char **, size_t);
int		 netproc(int, int, int, int, int, int, int, int,
			uid_t, gid_t, const char *const *, size_t);

/*
 * Warning and logging functions.
 * They should be used instead of err.h because they print the process
 * component and pid.
 */
void		 dowarnx(const char *, ...)
			__attribute__((format(printf, 1, 2)));
void		 dowarn(const char *, ...)
			__attribute__((format(printf, 1, 2)));
void		 doerr(const char *, ...)
			__attribute__((format(printf, 1, 2)));
void		 doerrx(const char *, ...)
			__attribute__((format(printf, 1, 2)));
void		 dodbg(const char *, ...)
			__attribute__((format(printf, 1, 2)));
void		 doddbg(const char *, ...)
			__attribute__((format(printf, 1, 2)));
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
void		 json_free_challenge(struct chng *);
int		 json_parse_challenge(struct json *, struct chng *);
void		 json_free_capaths(struct capaths *);
int		 json_parse_capaths(struct json *, struct capaths *);

char		*json_fmt_challenge(const char *, const char *);
char		*json_fmt_newauthz(const char *);
char		*json_fmt_newcert(const char *);
char		*json_fmt_newreg(const char *);
char		*json_fmt_protected(const char *, 
			const char *, const char *);
char		*json_fmt_revokecert(const char *);
char		*json_fmt_header(const char *, const char *);
char		*json_fmt_thumb(const char *, const char *);
char		*json_fmt_signed(const char *, 
			const char *, const char *, const char *);

int		 dropprivs(uid_t, gid_t);		 
int		 dropfs(const char *);

int		 sandbox_after();
int		 sandbox_before();

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
