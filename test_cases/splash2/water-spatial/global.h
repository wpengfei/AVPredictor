#line 228 "../null_macros/c.m4.null"

#line 1 "global.H"
/*************************************************************************/
/*                                                                       */
/*  Copyright (c) 1994 Stanford University                               */
/*                                                                       */
/*  All rights reserved.                                                 */
/*                                                                       */
/*  Permission is given to use, copy, and modify this software for any   */
/*  non-commercial purpose as long as this copyright notice is not       */
/*  removed.  All other uses, including redistribution in whole or in    */
/*  part, are forbidden without prior written permission.                */
/*                                                                       */
/*  This software is provided with absolutely no warranty and no         */
/*  support.                                                             */
/*                                                                       */
/*************************************************************************/

/*  This file contains the declaration of the GlobalMemory
structure and the maximum number of molecules allowed
by the program. */

struct GlobalMemory {
    pthread_mutex_t (IOLock);
    pthread_mutex_t (IndexLock);
    pthread_mutex_t (IntrafVirLock);
    pthread_mutex_t (InterfVirLock);
    pthread_mutex_t (KinetiSumLock);
    pthread_mutex_t (PotengSumLock);
    
#line 28
struct {
#line 28
	pthread_mutex_t	mutex;
#line 28
	pthread_cond_t	cv;
#line 28
	unsigned long	counter;
#line 28
	unsigned long	cycle;
#line 28
} (start);
#line 28

    
#line 29
struct {
#line 29
	pthread_mutex_t	mutex;
#line 29
	pthread_cond_t	cv;
#line 29
	unsigned long	counter;
#line 29
	unsigned long	cycle;
#line 29
} (InterfBar);
#line 29

    
#line 30
struct {
#line 30
	pthread_mutex_t	mutex;
#line 30
	pthread_cond_t	cv;
#line 30
	unsigned long	counter;
#line 30
	unsigned long	cycle;
#line 30
} (PotengBar);
#line 30

    long Index;
    double VIR;
    double SUM[3];
    double POTA, POTR, POTRF;
    unsigned long computestart,computeend;
    unsigned long trackstart, trackend, tracktime;
    unsigned long intrastart, intraend, intratime;
    unsigned long interstart, interend, intertime;
};

extern struct GlobalMemory *gl;

/* bndry.C */
void BNDRY(long ProcID);

/* cnstnt.C */
void CNSTNT(long N, double *C);

/* cshift.C */
void CSHIFT(double *XA, double *XB, double XMA, double XMB, double *XL, double BOXH, double BOXL);

/* initia.C */
void INITIA(void);
double xrand(double xl, double xh);

/* interf.C */
void INTERF(long DEST, double *VIR, long ProcID);
void UPDATE_FORCES(struct link *link_ptr, long DEST, double *XL, double *YL, double *ZL, double *FF);

/* intraf.C */
void INTRAF(double *VIR, long ProcID);

/* kineti.C */
void KINETI(double *SUM, double HMAS, double OMAS, long ProcID);

/* mdmain.C */
double MDMAIN(long NSTEP, long NPRINT, long NSAVE, long NORD1, long ProcID);

/* poteng.C */
void POTENG(double *POTA, double *POTR, double *PTRF, long ProcID);

/* predcor.C */
void PREDIC(double *C, long NOR1, long ProcID);
void CORREC(double *PCC, long NOR1, long ProcID);

/* syscons.C */
void SYSCNS(void);

/* water.C */
void WorkStart(void);
