#ifndef lint
static const char	RCSid[] = "$Id: rhoptimize.c,v 3.24 2023/02/06 22:40:21 greg Exp $";
#endif
/*
 * Optimize holodeck for quick access.
 *
 *	11/4/98		Greg Ward Larson
 */

#include <signal.h>
#include <string.h>
#include <stdio.h>

#include "platform.h"
#include "rterror.h"
#include "resolu.h"
#include "rtprocess.h" /* getpid() */
#include "holo.h"

#ifndef BKBSIZE
#define BKBSIZE		256		/* beam clump size (kilobytes) */
#endif

char	*progname;
char	tempfile[128];
int	dupchecking = 0;

static long rhinitcopy(int hfd[2], char *infn, char *outfn);
static int nuniq(RAYVAL *rva, int n);
static int bpcmp(const void *b1p, const void *b2p);
static int xferclump(HOLO *hp, int *bq, int nb);
static void copysect(int ifd, int ofd);


int
main(
	int	argc,
	char	*argv[]
)
{
	char	*inpname, *outname;
	int	hdfd[2];
	off_t	nextipos, lastopos, thisopos;

	progname = argv[0];
	argv++; argc--;			/* duplicate checking flag? */
	if (argc > 1 && !strcmp(argv[0], "-u")) {
		dupchecking++;
		argv++; argc--;
	}
	if ((argc < 1) | (argc > 2)) {
		fprintf(stderr, "Usage: %s [-u] input.hdk [output.hdk]\n",
				progname);
		exit(1);
	}
	inpname = argv[0];		/* get input file */
	argv++; argc--;
	if (argc == 1)			/* use given output file */
		outname = argv[0];
	else {				/* else use temporary file */
		if (access(inpname, R_OK|W_OK) < 0) {	/* check permissions */
			sprintf(errmsg, "cannot access \"%s\"", inpname);
			error(SYSTEM, errmsg);
		}
		strcpy(tempfile, inpname);
		if ((outname = strrchr(tempfile, '/')) != NULL)
			outname++;
		else
			outname = tempfile;
		sprintf(outname, "rho%d.hdk", getpid());
		outname = tempfile;
	}
					/* copy holodeck file header */
	nextipos = rhinitcopy(hdfd, inpname, outname);
	lastopos = 0;			/* copy sections one by one */
	while (nextipos != 0L) {
					/* set input position; get next */
		lseek(hdfd[0], nextipos, SEEK_SET);
		read(hdfd[0], (char *)&nextipos, sizeof(nextipos));
					/* get output position; set last */
		thisopos = lseek(hdfd[1], (off_t)0, SEEK_END);
		if (lastopos > 0) {
			lseek(hdfd[1], lastopos, SEEK_SET);
			write(hdfd[1], (char *)&thisopos, sizeof(thisopos));
			lseek(hdfd[1], (off_t)0, SEEK_END);
		}
		lastopos = thisopos;
		thisopos = 0;		/* write place holder */
		write(hdfd[1], (char *)&thisopos, sizeof(thisopos));
					/* copy holodeck section */
		copysect(hdfd[0], hdfd[1]);
	}
					/* clean up */
	close(hdfd[0]);
	close(hdfd[1]);
	if (outname == tempfile && rename(outname, inpname) < 0) {
		sprintf(errmsg, "cannot rename \"%s\" to \"%s\"",
				outname, inpname);
		error(SYSTEM, errmsg);
	}
	return(0);
}


static long
rhinitcopy(	/* open files and copy header */
	int	hfd[2],			/* returned file descriptors */
	char	*infn,
	char	*outfn
)
{
	FILE	*infp, *outfp;
	long	ifpos;
					/* open files for i/o */
	if ((infp = fopen(infn, "rb")) == NULL) {
		sprintf(errmsg, "cannot open \"%s\" for reading", infn);
		error(SYSTEM, errmsg);
	}
	if (access(outfn, F_OK) == 0) {
		sprintf(errmsg, "output file \"%s\" already exists!", outfn);
		error(USER, errmsg);
	}
	if ((outfp = fopen(outfn, "wb+")) == NULL) {
		sprintf(errmsg, "cannot open \"%s\" for writing", outfn);
		error(SYSTEM, errmsg);
	}
					/* set up signal handling */
#ifdef SIGINT
	if (signal(SIGINT, quit) == SIG_IGN) signal(SIGINT, SIG_IGN);
#endif
#ifdef SIGHUP
	if (signal(SIGHUP, quit) == SIG_IGN) signal(SIGHUP, SIG_IGN);
#endif
#ifdef SIGTERM
	if (signal(SIGTERM, quit) == SIG_IGN) signal(SIGTERM, SIG_IGN);
#endif
#ifdef SIGXCPU
	if (signal(SIGXCPU, quit) == SIG_IGN) signal(SIGXCPU, SIG_IGN);
	if (signal(SIGXFSZ, quit) == SIG_IGN) signal(SIGXFSZ, SIG_IGN);
#endif
					/* copy and verify header */
	if (checkheader(infp, HOLOFMT, outfp) < 0 || getw(infp) != HOLOMAGIC)
		error(USER, "input not in holodeck format");
	fputformat(HOLOFMT, outfp);
	fputc('\n', outfp);
	putw(HOLOMAGIC, outfp);
					/* get descriptors and free stdio */
	if ((hfd[0] = dup(fileno(infp))) < 0 ||
			(hfd[1] = dup(fileno(outfp))) < 0)
		error(SYSTEM, "dup call failed in rhinitcopy");
	ifpos = ftell(infp);
	fclose(infp);
	if (fclose(outfp) == EOF)
		error(SYSTEM, "file flushing error in rhinitcopy");
					/* check cache size */
	if (BKBSIZE*1024*1.5 > hdcachesize)
		hdcachesize = BKBSIZE*1024*1.5;
					/* return input position */
	return(ifpos);
}


static int
nuniq(			/* sort unique rays to front of beam list */
	RAYVAL	*rva,
	int	n
)
{
	int	i, j;
	RAYVAL	rtmp;

	for (j = 0; j < n; j++)
		for (i = j+1; i < n; i++)
			if ( rva[i].d == rva[j].d &&
					rva[i].r[0][0]==rva[j].r[0][0] &&
					rva[i].r[0][1]==rva[j].r[0][1] &&
					rva[i].r[1][0]==rva[j].r[1][0] &&
					rva[i].r[1][1]==rva[j].r[1][1] ) {
				n--;		/* swap duplicate with end */
				rtmp = *(rva+n);
				*(rva+n) = *(rva+i);
				*(rva+i) = rtmp;
				i--;		/* recheck one we swapped */
			}
	return(n);
}


static BEAMI	*beamdir;

static int
bpcmp(			/* compare beam positions on disk */
	const void	*b1p,
	const void	*b2p
)
{
	off_t	pdif = beamdir[*(int*)b1p].fo - beamdir[*(int*)b2p].fo;

	if (pdif < 0L) return(-1);
	return(pdif > 0L);
}

static HOLO	*hout;

static int
xferclump(		/* transfer the given clump to hout and free */
	HOLO	*hp,
	int	*bq,
	int	nb
)
{
	int	i;
	BEAM	*bp;
	int	n;

	beamdir = hp->bi;		/* sort based on file position */
	qsort((void *)bq, nb, sizeof(*bq), bpcmp);
					/* transfer and free each beam */
	for (i = 0; i < nb; i++) {
		bp = hdgetbeam(hp, bq[i]);
		DCHECK(bp==NULL, CONSISTENCY, "empty beam in xferclump");
		n = dupchecking ? nuniq(hdbray(bp),bp->nrm) : bp->nrm;
		memcpy((void *)hdnewrays(hout,bq[i],n),(void *)hdbray(bp), 
				n*sizeof(RAYVAL));
		hdfreebeam(hp, bq[i]);
	}
	hdfreebeam(hout, 0);		/* write & free clump */
	return(0);
}

static void
copysect(		/* copy holodeck section from ifd to ofd */
	int	ifd,
	int	ofd
)
{
	HOLO	*hinp;
					/* load input section directory */
	hinp = hdinit(ifd, NULL);
					/* create output section directory */
	hout = hdinit(ofd, (HDGRID *)hinp);
					/* clump the beams */
	clumpbeams(hinp, 0, BKBSIZE*1024, xferclump);
					/* clean up */
	hddone(hinp);
	hddone(hout);
}


void
eputs(const char *s)			/* put error message to stderr */
{
	static int  midline = 0;

	if (!*s)
		return;
	if (!midline++) {	/* prepend line with program name */
		fputs(progname, stderr);
		fputs(": ", stderr);
	}
	fputs(s, stderr);
	if (s[strlen(s)-1] == '\n') {
		fflush(stderr);
		midline = 0;
	}
}


void
quit(code)			/* exit the program gracefully */
int	code;
{
	if (tempfile[0])
		unlink(tempfile);
	exit(code);
}
