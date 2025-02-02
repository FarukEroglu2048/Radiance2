#ifndef lint
static const char	RCSid[] = "$Id: misc.c,v 1.6 2023/06/09 15:25:49 greg Exp $";
#endif
/*
 *   Miscellaneous functions for meta-files
 */


#include  "rtio.h"
#include  "meta.h"


char  coms[] = COML;

/* char  errmsg[128];	redundant to error.c in ../commmon */



int
comndx(		/* return index for command */
	int  c
)

{
 char  *cp;

 if (!isalpha(c))
    return(-1);

 for (cp = coms; *cp; cp++)
    if(*cp == c)
       return(cp-coms);

 return(-1);
 }






PRIMITIVE  *
pop(			/* pop top off plist */
	PLIST  *pl
)

{
 PRIMITIVE  *p;

 if ((p = pl->ptop) != NULL)  {
    if ((pl->ptop = p->pnext) == NULL)
       pl->pbot = NULL;
    p->pnext = NULL;
    }

 return(p);
 }



void
push(		/* push primitive onto plist */
	PRIMITIVE  *p,
	PLIST  *pl
)

{

 if ((p->pnext = pl->ptop) == NULL)
    pl->pbot = p;
 pl->ptop = p;

 }



void
add(		/* add primitive to plist */
	PRIMITIVE  *p,
	PLIST  *pl
)
{

 if (pl->ptop == NULL)
    pl->ptop = p;
 else
    pl->pbot->pnext = p;
 p->pnext = NULL;
 pl->pbot = p;

 }



void
append(		/* append pl1 to the end of pl2 */
	PLIST  *pl1,
	PLIST  *pl2
)

{

    if (pl1->ptop != NULL) {
        if (pl2->ptop != NULL)
            pl2->pbot->pnext = pl1->ptop;
        else
            pl2->ptop = pl1->ptop;
        pl2->pbot = pl1->pbot;
    }

}



void
fargs(		/* free any arguments p has */
PRIMITIVE  *p
)

{

 if (p->args != NULL) {
    freestr(p->args);
    p->args = NULL;
    }
    
 }



char *
nextscan(		/* scan and advance through string */
	char  *start,
	char  *format,
	char  *result
)

{

    if (start == NULL) return(NULL);
    
    while (isspace(*start)) start++;
    
    if (sscanf(start, format, result) != 1) return(NULL);
    
    while (*start && !isspace(*start)) start++;
    
    return(start);
}


void
mcopy(	/* copy p2 into p1 size n */
char  *p1,
char  *p2,
int  n
)

{

    while (n--)
	*p1++ = *p2++;

}

