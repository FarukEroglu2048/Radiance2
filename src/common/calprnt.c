#ifndef lint
static const char	RCSid[] = "$Id: calprnt.c,v 2.9 2024/02/24 19:00:23 greg Exp $";
#endif
/*
 *  calprint.c - routines for printing calcomp expressions.
 */

#include "copyright.h"

#include  <stdio.h>
#include  <string.h>

#include  "rterror.h"
#include  "calcomp.h"


/* is child operation lower precedence than parent? */
static int
lower_precedent_op(int typ, EPNODE *ek)
{
    if (ek == NULL)
	return(0);
    switch (typ) {
	case '+':
		return(0);
	case '-':
		return(ek->type == '+');
	case '*':
		return(strchr("+-", ek->type) != NULL);
	case '/':
		return(strchr("+-*", ek->type) != NULL);
	case '^':
		return(strchr("+-*/^", ek->type) != NULL);
	}
    return(0);			/* child not binary op */
}

void
eprint(				/* print a parse tree */
	EPNODE  *ep,
	FILE  *fp
)
{
    static EPNODE  *curdef = NULL;
    EPNODE  *ep1 = NULL;

    switch (ep==NULL ? -1 : ep->type) {

	case VAR:
	    fputs(ep->v.ln->name, fp);
	    break;

	case SYM:
	    fputs(ep->v.name, fp);
	    break;

	case FUNC:
	    eprint(ep->v.kid, fp);
	    fputc('(', fp);
	    ep1 = ep->v.kid->sibling;
	    while (ep1 != NULL) {
		eprint(ep1, fp);
		if ((ep1 = ep1->sibling) != NULL)
		    fputs(", ", fp);
	    }
	    fputc(')', fp);
	    break;

	case ARG:
	    if (curdef == NULL || curdef->v.kid->type != FUNC ||
			(ep1 = ekid(curdef->v.kid, ep->v.chan)) == NULL) {
		eputs("Bad argument!\n");
		quit(1);
	    }
	    eprint(ep1, fp);
	    break;

	case NUM:
	    fprintf(fp, "%.9g", ep->v.num);
	    break;

	case CHAN:
	    fprintf(fp, "$%d", ep->v.chan);
	    break;
	
	case '=':
	case ':':
	    ep1 = curdef;
	    curdef = ep;
	    eprint(ep->v.kid, fp);
	    fputc(' ', fp);
	    fputc(ep->type, fp);
	    fputc(' ', fp);
	    eprint(ep->v.kid->sibling, fp);
	    curdef = ep1;
	    break;

	case UMINUS:
	    fputc('-', fp);
	    if (ep->v.kid != NULL && strchr("+-*/^", ep->v.kid->type)) {
		fputc('(', fp);
		eprint(ep->v.kid, fp);
		fputc(')', fp);
	    } else
		eprint(ep->v.kid, fp);
	    break;

	case '+':
	case '*':
	case '-':
	case '/':
	case '^':
	    if (lower_precedent_op(ep->type, ep->v.kid)) {
		fputc('(', fp);
		eprint(ep->v.kid, fp);
		fputc(')', fp);
	    } else
		eprint(ep->v.kid, fp);
	    for (ep1 = ep->v.kid->sibling; ep1 != NULL; ep1 = ep1->sibling) {
	    	if (ep->type != '^') {
		    fputc(' ', fp);
		    fputc(ep->type, fp);
		    fputc(' ', fp);
	    	} else
		    fputc(ep->type, fp);
		if (lower_precedent_op(ep->type, ep1)) {
		    fputc('(', fp);
		    eprint(ep1, fp);
		    fputc(')', fp);
	    	} else
		    eprint(ep1, fp);
	    }
	    break;

	default:
	    eputs("Bad expression!\n");
	    quit(1);

    }
}


void
dprint(			/* print a definition (all if no name) */
	char  *name,
	FILE  *fp
)
{
    EPNODE  *ep;
    
    if (name == NULL)
	for (ep = dfirst(); ep != NULL; ep = dnext()) {
	    eprint(ep, fp);
	    fputs(";\n", fp);
	}
    else if ((ep = dlookup(name)) != NULL) {
	eprint(ep, fp);
	fputs(";\n", fp);
    } else {
	wputs(name);
	wputs(": undefined\n");
    }
}
