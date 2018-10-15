#line 228 "../null_macros/c.m4.null"

#line 1 "slave1.C"
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

/*    ****************
      subroutine slave
      ****************  */

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "decs.h"

void slave()
{
   long i;
   long j;
   long nstep;
   long iindex;
   long iday;
   double ysca1;
   double y;
   double factor;
   double sintemp;
   double curlt;
   double ressqr;
   long istart;
   long iend;
   long jstart;
   long jend;
   long ist;
   long ien;
   long jst;
   long jen;
   double fac;
   long dayflag=0;
   long dhourflag=0;
   long endflag=0;
   double ttime;
   double dhour;
   double day;
   long firstrow;
   long lastrow;
   long numrows;
   long firstcol;
   long lastcol;
   long numcols;
   long psiindex;
   double psibipriv;
   long psinum;
   long procid;
   unsigned long t1;

   ressqr = lev_res[numlev-1] * lev_res[numlev-1];

   {pthread_mutex_lock(&(locks->idlock));}
     procid = global->id;
     global->id = global->id+1;
   {pthread_mutex_unlock(&(locks->idlock));}

/* POSSIBLE ENHANCEMENT:  Here is where one might pin processes to
   processors to avoid migration. */

/* POSSIBLE ENHANCEMENT:  Here is where one might distribute
   data structures across physically distributed memories in
   a round-robin fashion. */

   firstcol = gp[procid].rel_start_x[numlev-1];
   lastcol = firstcol + gp[procid].rel_num_x[numlev-1] - 1;
   firstrow = gp[procid].rel_start_y[numlev-1];
   lastrow = firstrow + gp[procid].rel_num_y[numlev-1] - 1;
   numcols = gp[procid].rel_num_x[numlev-1];
   numrows = gp[procid].rel_num_y[numlev-1];

   if (procid > nprocs/2) {
      psinum = 2;
   } else {
      psinum = 1;
   }

/* every process gets its own copy of the timing variables to avoid
   contention at shared memory locations.  here, these variables
   are initialized.  */

   ttime = 0.0;
   dhour = 0.0;
   nstep = 0 ;
   day = 0.0;

   ysca1 = 0.5*ysca;
   if (procid == MASTER) {
     for(iindex = 0;iindex<=jm-1;iindex++) {
       y = ((double) iindex)*res;
       wrk2->f[iindex] = f0+beta*(y-ysca1);
     }
   }

   if (procid == MASTER) {
     fields2->psium[0][0]=0.0;
   }
   if (procid == nprocs-xprocs) {
     fields2->psium[im-1][0]=0.0;
   }
   if (procid == xprocs-1) {
     fields2->psium[0][jm-1]=0.0;
   }
   if (procid == nprocs-1) {
     fields2->psium[im-1][jm-1]=0.0;
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       fields2->psium[0][j] = 0.0;
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       fields2->psium[im-1][j] = 0.0;
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       fields2->psium[j][0] = 0.0;
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       fields2->psium[j][jm-1] = 0.0;
     }
   }

   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       fields2->psium[i][iindex] = 0.0;
     }
   }
   if (procid == MASTER) {
     fields2->psilm[0][0]=0.0;
   }
   if (procid == nprocs-xprocs) {
     fields2->psilm[im-1][0]=0.0;
   }
   if (procid == xprocs-1) {
     fields2->psilm[0][jm-1]=0.0;
   }
   if (procid == nprocs-1) {
     fields2->psilm[im-1][jm-1]=0.0;
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       fields2->psilm[0][j] = 0.0;
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       fields2->psilm[im-1][j] = 0.0;
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       fields2->psilm[j][0] = 0.0;
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       fields2->psilm[j][jm-1] = 0.0;
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       fields2->psilm[i][iindex] = 0.0;
     }
   }

   if (procid == MASTER) {
     wrk1->psib[0][0]=1.0;
   }
   if (procid == xprocs-1) {
     wrk1->psib[0][jm-1]=1.0;
   }
   if (procid == nprocs-xprocs) {
     wrk1->psib[im-1][0]=1.0;
   }
   if (procid == nprocs-1) {
     wrk1->psib[im-1][jm-1]=1.0;
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk1->psib[0][j] = 1.0;
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk1->psib[im-1][j] = 1.0;
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk1->psib[j][0] = 1.0;
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk1->psib[j][jm-1] = 1.0;
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       wrk1->psib[i][iindex] = 0.0;
     }
   }

/* wait until all processes have completed the above initialization  */
#if defined(MULTIPLE_BARRIERS)
{
#line 228
	unsigned long	Error, Cycle;
#line 228
	long		Cancel, Temp;
#line 228

#line 228
	Error = pthread_mutex_lock(&(bars->sl_prini).mutex);
#line 228
	if (Error != 0) {
#line 228
		printf("Error while trying to get lock in barrier.\n");
#line 228
		exit(-1);
#line 228
	}
#line 228

#line 228
	Cycle = (bars->sl_prini).cycle;
#line 228
	if (++(bars->sl_prini).counter != (nprocs)) {
#line 228
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 228
		while (Cycle == (bars->sl_prini).cycle) {
#line 228
			Error = pthread_cond_wait(&(bars->sl_prini).cv, &(bars->sl_prini).mutex);
#line 228
			if (Error != 0) {
#line 228
				break;
#line 228
			}
#line 228
		}
#line 228
		pthread_setcancelstate(Cancel, &Temp);
#line 228
	} else {
#line 228
		(bars->sl_prini).cycle = !(bars->sl_prini).cycle;
#line 228
		(bars->sl_prini).counter = 0;
#line 228
		Error = pthread_cond_broadcast(&(bars->sl_prini).cv);
#line 228
	}
#line 228
	pthread_mutex_unlock(&(bars->sl_prini).mutex);
#line 228
}
#else
{
#line 230
	unsigned long	Error, Cycle;
#line 230
	long		Cancel, Temp;
#line 230

#line 230
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 230
	if (Error != 0) {
#line 230
		printf("Error while trying to get lock in barrier.\n");
#line 230
		exit(-1);
#line 230
	}
#line 230

#line 230
	Cycle = (bars->barrier).cycle;
#line 230
	if (++(bars->barrier).counter != (nprocs)) {
#line 230
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 230
		while (Cycle == (bars->barrier).cycle) {
#line 230
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 230
			if (Error != 0) {
#line 230
				break;
#line 230
			}
#line 230
		}
#line 230
		pthread_setcancelstate(Cancel, &Temp);
#line 230
	} else {
#line 230
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 230
		(bars->barrier).counter = 0;
#line 230
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 230
	}
#line 230
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 230
}
#endif
   istart = gp[procid].rel_start_y[numlev-1];
   iend = istart + gp[procid].rel_num_y[numlev-1] - 1;
   jstart = gp[procid].rel_start_x[numlev-1];
   jend = jstart + gp[procid].rel_num_x[numlev-1] - 1;
   ist = istart;
   ien = iend;
   jst = jstart;
   jen = jend;
   if (istart == 1) {
     istart = 0;
   }
   if (jstart == 1) {
     jstart = 0;
   }
   if (iend == im-2) {
     iend = im-1;
   }
   if (jend == jm-2) {
     jend = jm-1;
   }
   for(i=istart;i<=iend;i++) {
     for(j=jstart;j<=jend;j++) {
       multi->rhs_multi[numlev-1][i][j] = wrk1->psib[i][j] * ressqr;
     }
   }
   if (istart == 0) {
     for(j=jstart;j<=jend;j++) {
       multi->q_multi[numlev-1][0][j] = wrk1->psib[0][j];
     }
   }
   if (iend == im-1) {
     for(j=jstart;j<=jend;j++) {
       multi->q_multi[numlev-1][im-1][j] = wrk1->psib[im-1][j];
     }
   }
   if (jstart == 0) {
     for(i=istart;i<=iend;i++) {
       multi->q_multi[numlev-1][i][0] = wrk1->psib[i][0];
     }
   }
   if (jend == jm-1) {
     for(i=istart;i<=iend;i++) {
       multi->q_multi[numlev-1][i][jm-1] = wrk1->psib[i][jm-1];
     }
   }

   fac = 1.0 / (4.0 - ressqr*eig2);
   for(i=ist;i<=ien;i++) {
     for(j=jst;j<=jen;j++) {
       multi->q_multi[numlev-1][i][j] = fac * (wrk1->psib[i+1][j] +
           wrk1->psib[i-1][j] + wrk1->psib[i][j+1] + wrk1->psib[i][j-1] -
           ressqr*wrk1->psib[i][j]);
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 287
	unsigned long	Error, Cycle;
#line 287
	long		Cancel, Temp;
#line 287

#line 287
	Error = pthread_mutex_lock(&(bars->sl_prini).mutex);
#line 287
	if (Error != 0) {
#line 287
		printf("Error while trying to get lock in barrier.\n");
#line 287
		exit(-1);
#line 287
	}
#line 287

#line 287
	Cycle = (bars->sl_prini).cycle;
#line 287
	if (++(bars->sl_prini).counter != (nprocs)) {
#line 287
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 287
		while (Cycle == (bars->sl_prini).cycle) {
#line 287
			Error = pthread_cond_wait(&(bars->sl_prini).cv, &(bars->sl_prini).mutex);
#line 287
			if (Error != 0) {
#line 287
				break;
#line 287
			}
#line 287
		}
#line 287
		pthread_setcancelstate(Cancel, &Temp);
#line 287
	} else {
#line 287
		(bars->sl_prini).cycle = !(bars->sl_prini).cycle;
#line 287
		(bars->sl_prini).counter = 0;
#line 287
		Error = pthread_cond_broadcast(&(bars->sl_prini).cv);
#line 287
	}
#line 287
	pthread_mutex_unlock(&(bars->sl_prini).mutex);
#line 287
}
#else
   {
#line 289
	unsigned long	Error, Cycle;
#line 289
	long		Cancel, Temp;
#line 289

#line 289
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 289
	if (Error != 0) {
#line 289
		printf("Error while trying to get lock in barrier.\n");
#line 289
		exit(-1);
#line 289
	}
#line 289

#line 289
	Cycle = (bars->barrier).cycle;
#line 289
	if (++(bars->barrier).counter != (nprocs)) {
#line 289
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 289
		while (Cycle == (bars->barrier).cycle) {
#line 289
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 289
			if (Error != 0) {
#line 289
				break;
#line 289
			}
#line 289
		}
#line 289
		pthread_setcancelstate(Cancel, &Temp);
#line 289
	} else {
#line 289
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 289
		(bars->barrier).counter = 0;
#line 289
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 289
	}
#line 289
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 289
}
#endif
   multig(procid);

   for(i=istart;i<=iend;i++) {
     for(j=jstart;j<=jend;j++) {
       wrk1->psib[i][j] = multi->q_multi[numlev-1][i][j];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 299
	unsigned long	Error, Cycle;
#line 299
	long		Cancel, Temp;
#line 299

#line 299
	Error = pthread_mutex_lock(&(bars->sl_psini).mutex);
#line 299
	if (Error != 0) {
#line 299
		printf("Error while trying to get lock in barrier.\n");
#line 299
		exit(-1);
#line 299
	}
#line 299

#line 299
	Cycle = (bars->sl_psini).cycle;
#line 299
	if (++(bars->sl_psini).counter != (nprocs)) {
#line 299
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 299
		while (Cycle == (bars->sl_psini).cycle) {
#line 299
			Error = pthread_cond_wait(&(bars->sl_psini).cv, &(bars->sl_psini).mutex);
#line 299
			if (Error != 0) {
#line 299
				break;
#line 299
			}
#line 299
		}
#line 299
		pthread_setcancelstate(Cancel, &Temp);
#line 299
	} else {
#line 299
		(bars->sl_psini).cycle = !(bars->sl_psini).cycle;
#line 299
		(bars->sl_psini).counter = 0;
#line 299
		Error = pthread_cond_broadcast(&(bars->sl_psini).cv);
#line 299
	}
#line 299
	pthread_mutex_unlock(&(bars->sl_psini).mutex);
#line 299
}
#else
   {
#line 301
	unsigned long	Error, Cycle;
#line 301
	long		Cancel, Temp;
#line 301

#line 301
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 301
	if (Error != 0) {
#line 301
		printf("Error while trying to get lock in barrier.\n");
#line 301
		exit(-1);
#line 301
	}
#line 301

#line 301
	Cycle = (bars->barrier).cycle;
#line 301
	if (++(bars->barrier).counter != (nprocs)) {
#line 301
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 301
		while (Cycle == (bars->barrier).cycle) {
#line 301
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 301
			if (Error != 0) {
#line 301
				break;
#line 301
			}
#line 301
		}
#line 301
		pthread_setcancelstate(Cancel, &Temp);
#line 301
	} else {
#line 301
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 301
		(bars->barrier).counter = 0;
#line 301
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 301
	}
#line 301
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 301
}
#endif
/* update the local running sum psibipriv by summing all the resulting
   values in that process's share of the psib matrix   */

   psibipriv=0.0;
   if (procid == MASTER) {
     psibipriv = psibipriv + 0.25*(wrk1->psib[0][0]);
   }
   if (procid == xprocs-1){
     psibipriv = psibipriv + 0.25*(wrk1->psib[0][jm-1]);
   }
   if (procid == nprocs - xprocs) {
     psibipriv=psibipriv+0.25*(wrk1->psib[im-1][0]);
   }
   if (procid == nprocs-1) {
     psibipriv=psibipriv+0.25*(wrk1->psib[im-1][jm-1]);
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       psibipriv = psibipriv + 0.5*wrk1->psib[0][j];
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       psibipriv = psibipriv + 0.5*wrk1->psib[im-1][j];
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       psibipriv = psibipriv + 0.5*wrk1->psib[j][0];
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       psibipriv = psibipriv + 0.5*wrk1->psib[j][jm-1];
     }
   }
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
   for(i=firstrow;i<=lastrow;i++) {
       psibipriv = psibipriv + wrk1->psib[i][iindex];
     }
   }

/* update the shared variable psibi by summing all the psibiprivs
   of the individual processes into it.  note that this combined
   private and shared sum method avoids accessing the shared
   variable psibi once for every element of the matrix.  */

   {pthread_mutex_lock(&(locks->psibilock));}
   global->psibi = global->psibi + psibipriv;
   {pthread_mutex_unlock(&(locks->psibilock));}

   for(psiindex=0;psiindex<=1;psiindex++) {
     if (procid == MASTER) {
       fields->psim[psiindex][0][0] = 0.0;
     }
     if (procid == nprocs-xprocs) {
       fields->psim[psiindex][im-1][0] = 0.0;
     }
     if (procid == xprocs-1) {
       fields->psim[psiindex][0][jm-1] = 0.0;
     }
     if (procid == nprocs-1) {
       fields->psim[psiindex][im-1][jm-1] = 0.0;
     }
     if (firstrow == 1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields->psim[psiindex][0][j] = 0.0;
       }
     }
     if ((firstrow+numrows) == im-1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields->psim[psiindex][im-1][j] = 0.0;
       }
     }
     if (firstcol == 1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields->psim[psiindex][j][0] = 0.0;
       }
     }
     if ((firstcol+numcols) == jm-1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields->psim[psiindex][j][jm-1] = 0.0;
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
           fields->psim[psiindex][i][iindex] = 0.0;
       }
     }
   }

/* initialize psi matrices the same way  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     if (procid == MASTER) {
       fields->psi[psiindex][0][0] = 0.0;
     }
     if (procid == xprocs-1) {
       fields->psi[psiindex][0][jm-1] = 0.0;
     }
     if (procid == nprocs-xprocs) {
       fields->psi[psiindex][im-1][0] = 0.0;
     }
     if (procid == nprocs-1) {
       fields->psi[psiindex][im-1][jm-1] = 0.0;
     }
     if (firstrow == 1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields->psi[psiindex][0][j] = 0.0;
       }
     }
     if ((firstrow+numrows) == im-1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields->psi[psiindex][im-1][j] = 0.0;
       }
     }
     if (firstcol == 1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields->psi[psiindex][j][0] = 0.0;
       }
     }
     if ((firstcol+numcols) == jm-1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields->psi[psiindex][j][jm-1] = 0.0;
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         fields->psi[psiindex][i][iindex] = 0.0;
       }
     }
   }

/* compute input curl of wind stress */

   ysca1 = .5*ysca;
   factor= -t0*pi/ysca1;
   if (procid == MASTER) {
     frcng->tauz[0][0] = 0.0;
   }
   if (procid == nprocs-xprocs) {
     frcng->tauz[im-1][0] = 0.0;
   }
   if (procid == xprocs-1) {
     sintemp = pi*((double) jmm1)*res/ysca1;
     sintemp = sin(sintemp);
     frcng->tauz[0][jm-1] = factor*sintemp;
   }
   if (procid == nprocs-1) {
     sintemp = pi*((double) jmm1)*res/ysca1;
     sintemp = sin(sintemp);
     frcng->tauz[im-1][jm-1] = frcng->tauz[0][jm-1];
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       sintemp = pi*((double) j)*res/ysca1;
       sintemp = sin(sintemp);
       curlt = factor*sintemp;
       frcng->tauz[0][j] = curlt;
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       sintemp = pi*((double) j)*res/ysca1;
       sintemp = sin(sintemp);
       curlt = factor*sintemp;
       frcng->tauz[im-1][j] = curlt;
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       frcng->tauz[j][0] = 0.0;
     }
   }
   if ((firstcol+numcols) == jm-1) {
     sintemp = pi*((double) jmm1)*res/ysca1;
     sintemp = sin(sintemp);
     curlt = factor*sintemp;
     for(j=firstrow;j<=lastrow;j++) {
       frcng->tauz[j][jm-1] = curlt;
     }
   }
   for(iindex=firstcol;iindex<=lastcol;iindex++) {
     sintemp = pi*((double) iindex)*res/ysca1;
     sintemp = sin(sintemp);
     curlt = factor*sintemp;
     for(i=firstrow;i<=lastrow;i++) {
       frcng->tauz[i][iindex] = curlt;
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 494
	unsigned long	Error, Cycle;
#line 494
	long		Cancel, Temp;
#line 494

#line 494
	Error = pthread_mutex_lock(&(bars->sl_onetime).mutex);
#line 494
	if (Error != 0) {
#line 494
		printf("Error while trying to get lock in barrier.\n");
#line 494
		exit(-1);
#line 494
	}
#line 494

#line 494
	Cycle = (bars->sl_onetime).cycle;
#line 494
	if (++(bars->sl_onetime).counter != (nprocs)) {
#line 494
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 494
		while (Cycle == (bars->sl_onetime).cycle) {
#line 494
			Error = pthread_cond_wait(&(bars->sl_onetime).cv, &(bars->sl_onetime).mutex);
#line 494
			if (Error != 0) {
#line 494
				break;
#line 494
			}
#line 494
		}
#line 494
		pthread_setcancelstate(Cancel, &Temp);
#line 494
	} else {
#line 494
		(bars->sl_onetime).cycle = !(bars->sl_onetime).cycle;
#line 494
		(bars->sl_onetime).counter = 0;
#line 494
		Error = pthread_cond_broadcast(&(bars->sl_onetime).cv);
#line 494
	}
#line 494
	pthread_mutex_unlock(&(bars->sl_onetime).mutex);
#line 494
}
#else
   {
#line 496
	unsigned long	Error, Cycle;
#line 496
	long		Cancel, Temp;
#line 496

#line 496
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 496
	if (Error != 0) {
#line 496
		printf("Error while trying to get lock in barrier.\n");
#line 496
		exit(-1);
#line 496
	}
#line 496

#line 496
	Cycle = (bars->barrier).cycle;
#line 496
	if (++(bars->barrier).counter != (nprocs)) {
#line 496
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 496
		while (Cycle == (bars->barrier).cycle) {
#line 496
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 496
			if (Error != 0) {
#line 496
				break;
#line 496
			}
#line 496
		}
#line 496
		pthread_setcancelstate(Cancel, &Temp);
#line 496
	} else {
#line 496
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 496
		(bars->barrier).counter = 0;
#line 496
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 496
	}
#line 496
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 496
}
#endif

/***************************************************************
 one-time stuff over at this point
 ***************************************************************/

   while (!endflag) {
     while ((!dayflag) || (!dhourflag)) {
       dayflag = 0;
       dhourflag = 0;
       if (nstep == 1) {
         if (procid == MASTER) {
            {
#line 509
	struct timeval	FullTime;
#line 509

#line 509
	gettimeofday(&FullTime, NULL);
#line 509
	(global->trackstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 509
}
         }
         if ((procid == MASTER) || (do_stats)) {
           {
#line 512
	struct timeval	FullTime;
#line 512

#line 512
	gettimeofday(&FullTime, NULL);
#line 512
	(t1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 512
};
           gp[procid].total_time = t1;
           gp[procid].multi_time = 0;
         }
/* POSSIBLE ENHANCEMENT:  Here is where one might reset the
   statistics that one is measuring about the parallel execution */
       }

       slave2(procid,firstrow,lastrow,numrows,firstcol,lastcol,numcols);

/* update time and step number
   note that these time and step variables are private i.e. every
   process has its own copy and keeps track of its own time  */

       ttime = ttime + dtau;
       nstep = nstep + 1;
       day = ttime/86400.0;
       if (day > ((double) outday0)) {
         dayflag = 1;
         iday = (long) day;
         dhour = dhour+dtau;
         if (dhour >= 86400.0) {
	   dhourflag = 1;
         }
       }
     }
     dhour = 0.0;

/* update values of psium array to psium + psim{1}  */

     if (procid == MASTER) {
       fields2->psium[0][0] = fields2->psium[0][0]+fields->psim[0][0][0];
     }
     if (procid == nprocs-xprocs) {
       fields2->psium[im-1][0] = fields2->psium[im-1][0]+fields->psim[0][im-1][0];
     }
     if (procid == xprocs-1) {
       fields2->psium[0][jm-1] = fields2->psium[0][jm-1]+fields->psim[0][0][jm-1];
     }
     if (procid == nprocs-1) {
       fields2->psium[im-1][jm-1] = fields2->psium[im-1][jm-1]+fields->psim[0][im-1][jm-1];
     }
     if (firstrow == 1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields2->psium[0][j] = fields2->psium[0][j]+fields->psim[0][0][j];
       }
     }
     if ((firstrow+numrows) == im-1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields2->psium[im-1][j] = fields2->psium[im-1][j]+fields->psim[0][im-1][j];
       }
     }
     if (firstcol == 1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields2->psium[j][0] = fields2->psium[j][0]+fields->psim[0][j][0];
       }
     }
     if ((firstcol+numcols) == jm-1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields2->psium[j][jm-1] = fields2->psium[j][jm-1]+fields->psim[0][j][jm-1];
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         fields2->psium[i][iindex] = fields2->psium[i][iindex]+fields->psim[0][i][iindex];
       }
     }

/* update values of psilm array to psilm + psim[2]  */

     if (procid == MASTER) {
       fields2->psilm[0][0] = fields2->psilm[0][0]+fields->psim[1][0][0];
     }
     if (procid == nprocs-xprocs) {
       fields2->psilm[im-1][0] = fields2->psilm[im-1][0]+fields->psim[1][im-1][0];
     }
     if (procid == xprocs-1) {
       fields2->psilm[0][jm-1] = fields2->psilm[0][jm-1]+fields->psim[1][0][jm-1];
     }
     if (procid == nprocs-1) {
       fields2->psilm[im-1][jm-1] = fields2->psilm[im-1][jm-1]+fields->psim[1][im-1][jm-1];
     }
     if (firstrow == 1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields2->psilm[0][j] = fields2->psilm[0][j]+fields->psim[1][0][j];
       }
     }
     if ((firstrow+numrows) == im-1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields2->psilm[im-1][j] = fields2->psilm[im-1][j]+fields->psim[1][im-1][j];
       }
     }
     if (firstcol == 1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields2->psilm[j][0] = fields2->psilm[j][0]+fields->psim[1][j][0];
       }
     }
     if ((firstcol+numcols) == jm-1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields2->psilm[j][jm-1] = fields2->psilm[j][jm-1]+fields->psim[1][j][jm-1];
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         fields2->psilm[i][iindex] = fields2->psilm[i][iindex]+fields->psim[1][i][iindex];
       }
     }
     if (iday >= (long) outday3) {
       endflag = 1;
     }
  }
  if ((procid == MASTER) || (do_stats)) {
    {
#line 624
	struct timeval	FullTime;
#line 624

#line 624
	gettimeofday(&FullTime, NULL);
#line 624
	(t1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 624
};
    gp[procid].total_time = t1-gp[procid].total_time;
  }
}
