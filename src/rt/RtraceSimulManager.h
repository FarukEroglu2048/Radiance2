/* RCSid $Id: RtraceSimulManager.h,v 2.1 2023/02/08 17:41:48 greg Exp $ */
/*
 *  RtraceSimulManager.h
 *
 *	Rtrace simulation manager class declaration (along with base class)
 *	Enqueuing rays will block caller iff #rays >= ThreadsAvail()
 *	Reporting call-backs made from EnqueBundle() and FlushQueue()
 *
 *  Created by Greg Ward on 11/10/22.
 */

#ifndef RtraceSimulManager_h
#define RtraceSimulManager_h

#include "ray.h"

extern char *	octname;	// global octree name

/// Ray reporting callback method
typedef void	RayReportCall(RAY *r, void *cd);

/// Multi-threaded simulation manager base class
class RadSimulManager {
	int			nThreads;	// number of active threads
public:
				RadSimulManager(const char *octn = NULL) {
					LoadOctree(octn);
					nThreads = 1;
				}
				~RadSimulManager() {
					Cleanup();
				}
				/// Load octree and prepare renderer
	bool			LoadOctree(const char *octn);
				/// Set number of computation threads (0 => #cores)
	int			SetThreadCount(int nt = 0);
				/// Check thread count (1 means no multi-threading)
	int			NThreads() const {
					return nThreads;
				}
				/// How many threads are currently unoccupied?
	int			ThreadsAvailable() const;
				/// Are we ready?
	bool			Ready() const {
					return (octname && nsceneobjs > 0);
				}
				/// Close octree, free data, return status
	int			Cleanup();
};

/// Flags to control rendering operations
enum {RTdoFIFO=1, RTtraceSources=2, RTlimDist=4, RTimmIrrad=8, RTmask=15};

/// rtrace-like simulation manager (at most one such object)
class RtraceSimulManager : public RadSimulManager {
	RayReportCall *		cookedCall;	// callback for cooked primary rays
	void *			ccData;		// client data for cooked primary rays
	RayReportCall *		traceCall;	// call for every ray in tree
	void *			tcData;		// client data for traced rays
	int			curFlags;	// current operating flags
				// Call-back for global ray-tracing context
	static void		RTracer(RAY *r);
				// Check for changes to render flags, etc.
	bool			UpdateMode();
protected:
	RNUMBER			lastRayID;	// last ray ID assigned
public:
	int			rtFlags;	// operation (RT*) flags
				RtraceSimulManager(RayReportCall *cb = NULL, void *cd = NULL,
						const char *octn = NULL) : RadSimulManager(octn) {
					lastRayID = 0;
					rtFlags = curFlags = 0;
					SetCookedCall(cb, cd);
					traceCall = NULL; tcData = NULL;
				}
				~RtraceSimulManager() {}
				/// Add ray bundle to queue w/ optional 1st ray ID
	int			EnqueueBundle(const FVECT orig_direc[], int n,
						RNUMBER rID0 = 0);
				/// Enqueue a single ray w/ optional ray ID
	bool			EnqueueRay(const FVECT org, const FVECT dir,
						RNUMBER rID = 0) {
					if (dir == org+1)
						return EnqueueBundle((const FVECT *)org, 1, rID);
					FVECT	orgdir[2];
					VCOPY(orgdir[0], org); VCOPY(orgdir[1], dir);
					return EnqueueBundle(orgdir, 1, rID);
				}
				/// Set/change cooked ray callback & FIFO flag
	void			SetCookedCall(RayReportCall *cb, void *cd = NULL) {
					if (cookedCall && (cookedCall != cb) | (ccData != cd))
						FlushQueue();
					cookedCall = cb;
					ccData = cd;
				}
				/// Set/change trace callback
	void			SetTraceCall(RayReportCall *cb, void *cd = NULL) {
					traceCall = cb;
					tcData = cd;
				}
				/// Are we ready?
	bool			Ready() const {
					return (cookedCall != NULL) | (traceCall != NULL) &&
						RadSimulManager::Ready();
				}
				/// Finish pending rays and complete callbacks
	bool			FlushQueue();
				/// Close octree, free data, return status
	int			Cleanup() {
					SetCookedCall(NULL);
					SetTraceCall(NULL);
					rtFlags = 0;
					UpdateMode();
					lastRayID = 0;
					return RadSimulManager::Cleanup();
				}
};

/// Determine if vector is all zeroes
inline bool
IsZeroVec(const FVECT vec)
{
	return (vec[0] == 0.0) & (vec[1] == 0.0) & (vec[2] == 0.0);
}

#endif /* RtraceSimulManager_h */
