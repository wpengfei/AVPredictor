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


#line 21
#include <pthread.h>
#line 21
#include <sys/time.h>
#line 21
#include <unistd.h>
#line 21
#include <stdlib.h>
#line 21
extern pthread_t PThreadTable[];
#line 21


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
   long firstrow;
   long lastrow;
   long numrows;
   long firstcol;
   long lastcol;
   long numcols;
   long psiindex;
   double psibipriv;
   double ttime;
   double dhour;
   double day;
   long procid;
   long psinum;
   long j_off = 0;
   unsigned long t1;
   double **t2a;
   double **t2b;
   double *t1a;
   double *t1b;
   double *t1c;
   double *t1d;

   ressqr = lev_res[numlev-1] * lev_res[numlev-1];

   {pthread_mutex_lock(&(locks->idlock));}
     procid = global->id;
     global->id = global->id+1;
   {pthread_mutex_unlock(&(locks->idlock));}

#if defined(MULTIPLE_BARRIERS)
   {
#line 84
	unsigned long	Error, Cycle;
#line 84
	long		Cancel, Temp;
#line 84

#line 84
	Error = pthread_mutex_lock(&(bars->sl_prini).mutex);
#line 84
	if (Error != 0) {
#line 84
		printf("Error while trying to get lock in barrier.\n");
#line 84
		exit(-1);
#line 84
	}
#line 84

#line 84
	Cycle = (bars->sl_prini).cycle;
#line 84
	if (++(bars->sl_prini).counter != (nprocs)) {
#line 84
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 84
		while (Cycle == (bars->sl_prini).cycle) {
#line 84
			Error = pthread_cond_wait(&(bars->sl_prini).cv, &(bars->sl_prini).mutex);
#line 84
			if (Error != 0) {
#line 84
				break;
#line 84
			}
#line 84
		}
#line 84
		pthread_setcancelstate(Cancel, &Temp);
#line 84
	} else {
#line 84
		(bars->sl_prini).cycle = !(bars->sl_prini).cycle;
#line 84
		(bars->sl_prini).counter = 0;
#line 84
		Error = pthread_cond_broadcast(&(bars->sl_prini).cv);
#line 84
	}
#line 84
	pthread_mutex_unlock(&(bars->sl_prini).mutex);
#line 84
}
#else
   {
#line 86
	unsigned long	Error, Cycle;
#line 86
	long		Cancel, Temp;
#line 86

#line 86
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 86
	if (Error != 0) {
#line 86
		printf("Error while trying to get lock in barrier.\n");
#line 86
		exit(-1);
#line 86
	}
#line 86

#line 86
	Cycle = (bars->barrier).cycle;
#line 86
	if (++(bars->barrier).counter != (nprocs)) {
#line 86
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 86
		while (Cycle == (bars->barrier).cycle) {
#line 86
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 86
			if (Error != 0) {
#line 86
				break;
#line 86
			}
#line 86
		}
#line 86
		pthread_setcancelstate(Cancel, &Temp);
#line 86
	} else {
#line 86
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 86
		(bars->barrier).counter = 0;
#line 86
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 86
	}
#line 86
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 86
}
#endif
/* POSSIBLE ENHANCEMENT:  Here is where one might pin processes to
   processors to avoid migration. */

/* POSSIBLE ENHANCEMENT:  Here is where one might distribute
   data structures across physically distributed memories as
   desired.

   One way to do this is as follows.  The function allocate(START,SIZE,I)
   is assumed to place all addresses x such that
   (START <= x < START+SIZE) on node I.

   long d_size;
   unsigned long g_size;
   unsigned long mg_size;

   if (procid == MASTER) {
     g_size = ((jmx[numlev-1]-2)/xprocs+2)*((imx[numlev-1]-2)/yprocs+2)*siz
eof(double) +
              ((imx[numlev-1]-2)/yprocs+2)*sizeof(double *);

     mg_size = numlev*sizeof(double **);
     for (i=0;i<numlev;i++) {
       mg_size+=((imx[i]-2)/yprocs+2)*((jmx[i]-2)/xprocs+2)*sizeof(double)+
                ((imx[i]-2)/yprocs+2)*sizeof(double *);
     }
     for (i= 0;i<nprocs;i++) {
       d_size = 2*sizeof(double **);
       allocate((unsigned long) psi[i],d_size,i);
       allocate((unsigned long) psim[i],d_size,i);
       allocate((unsigned long) work1[i],d_size,i);
       allocate((unsigned long) work4[i],d_size,i);
       allocate((unsigned long) work5[i],d_size,i);
       allocate((unsigned long) work7[i],d_size,i);
       allocate((unsigned long) temparray[i],d_size,i);
       allocate((unsigned long) psi[i][0],g_size,i);
       allocate((unsigned long) psi[i][1],g_size,i);
       allocate((unsigned long) psim[i][0],g_size,i);
       allocate((unsigned long) psim[i][1],g_size,i);
       allocate((unsigned long) psium[i],g_size,i);
       allocate((unsigned long) psilm[i],g_size,i);
       allocate((unsigned long) psib[i],g_size,i);
       allocate((unsigned long) ga[i],g_size,i);
       allocate((unsigned long) gb[i],g_size,i);
       allocate((unsigned long) work1[i][0],g_size,i);
       allocate((unsigned long) work1[i][1],g_size,i);
       allocate((unsigned long) work2[i],g_size,i);
       allocate((unsigned long) work3[i],g_size,i);
       allocate((unsigned long) work4[i][0],g_size,i);
       allocate((unsigned long) work4[i][1],g_size,i);
       allocate((unsigned long) work5[i][0],g_size,i);
       allocate((unsigned long) work5[i][1],g_size,i);
       allocate((unsigned long) work6[i],g_size,i);
       allocate((unsigned long) work7[i][0],g_size,i);
       allocate((unsigned long) work7[i][1],g_size,i);
       allocate((unsigned long) temparray[i][0],g_size,i);
       allocate((unsigned long) temparray[i][1],g_size,i);
       allocate((unsigned long) tauz[i],g_size,i);
       allocate((unsigned long) oldga[i],g_size,i);
       allocate((unsigned long) oldgb[i],g_size,i);
       d_size = numlev * sizeof(long);
       allocate((unsigned long) gp[i].rel_num_x,d_size,i);
       allocate((unsigned long) gp[i].rel_num_y,d_size,i);
       allocate((unsigned long) gp[i].eist,d_size,i);
       allocate((unsigned long) gp[i].ejst,d_size,i);
       allocate((unsigned long) gp[i].oist,d_size,i);
       allocate((unsigned long) gp[i].ojst,d_size,i);
       allocate((unsigned long) gp[i].rlist,d_size,i);
       allocate((unsigned long) gp[i].rljst,d_size,i);
       allocate((unsigned long) gp[i].rlien,d_size,i);
       allocate((unsigned long) gp[i].rljen,d_size,i);

       allocate((unsigned long) q_multi[i],mg_size,i);
       allocate((unsigned long) rhs_multi[i],mg_size,i);
       allocate((unsigned long) &(gp[i]),sizeof(struct Global_Private),i);
     }
   }

*/

   t2a = (double **) oldga[procid];
   t2b = (double **) oldgb[procid];
   for (i=0;i<im;i++) {
     t1a = (double *) t2a[i];
     t1b = (double *) t2b[i];
     for (j=0;j<jm;j++) {
        t1a[j] = 0.0;
        t1b[j] = 0.0;
     }
   }

   firstcol = 1;
   lastcol = firstcol + gp[procid].rel_num_x[numlev-1] - 1;
   firstrow = 1;
   lastrow = firstrow + gp[procid].rel_num_y[numlev-1] - 1;
   numcols = gp[procid].rel_num_x[numlev-1];
   numrows = gp[procid].rel_num_y[numlev-1];
   j_off = gp[procid].colnum*numcols;

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
     t1a = (double *) f;
     for (iindex = 0;iindex<=jmx[numlev-1]-1;iindex++) {
       y = ((double) iindex)*res;
       t1a[iindex] = f0+beta*(y-ysca1);
     }
   }

   t2a = (double **) psium[procid];
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[0][0]=0.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[im-1][0]=0.0;
   }
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[0][jm-1]=0.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[im-1][jm-1]=0.0;
   }
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 0.0;
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 0.0;
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][0] = 0.0;
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][jm-1] = 0.0;
     }
   }

   for(i=firstrow;i<=lastrow;i++) {
     t1a = (double *) t2a[i];
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       t1a[iindex] = 0.0;
     }
   }
   t2a = (double **) psilm[procid];
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[0][0]=0.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[im-1][0]=0.0;
   }
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[0][jm-1]=0.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[im-1][jm-1]=0.0;
   }
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 0.0;
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 0.0;
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][0] = 0.0;
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][jm-1] = 0.0;
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     t1a = (double *) t2a[i];
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       t1a[iindex] = 0.0;
     }
   }

   t2a = (double **) psib[procid];
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[0][0]=1.0;
   }
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[0][jm-1]=1.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[im-1][0]=1.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[im-1][jm-1]=1.0;
   }
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 1.0;
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 1.0;
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][0] = 1.0;
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][jm-1] = 1.0;
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     t1a = (double *) t2a[i];
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       t1a[iindex] = 0.0;
     }
   }

/* wait until all processes have completed the above initialization  */
#if defined(MULTIPLE_BARRIERS)
   {
#line 338
	unsigned long	Error, Cycle;
#line 338
	long		Cancel, Temp;
#line 338

#line 338
	Error = pthread_mutex_lock(&(bars->sl_prini).mutex);
#line 338
	if (Error != 0) {
#line 338
		printf("Error while trying to get lock in barrier.\n");
#line 338
		exit(-1);
#line 338
	}
#line 338

#line 338
	Cycle = (bars->sl_prini).cycle;
#line 338
	if (++(bars->sl_prini).counter != (nprocs)) {
#line 338
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 338
		while (Cycle == (bars->sl_prini).cycle) {
#line 338
			Error = pthread_cond_wait(&(bars->sl_prini).cv, &(bars->sl_prini).mutex);
#line 338
			if (Error != 0) {
#line 338
				break;
#line 338
			}
#line 338
		}
#line 338
		pthread_setcancelstate(Cancel, &Temp);
#line 338
	} else {
#line 338
		(bars->sl_prini).cycle = !(bars->sl_prini).cycle;
#line 338
		(bars->sl_prini).counter = 0;
#line 338
		Error = pthread_cond_broadcast(&(bars->sl_prini).cv);
#line 338
	}
#line 338
	pthread_mutex_unlock(&(bars->sl_prini).mutex);
#line 338
}
#else
   {
#line 340
	unsigned long	Error, Cycle;
#line 340
	long		Cancel, Temp;
#line 340

#line 340
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 340
	if (Error != 0) {
#line 340
		printf("Error while trying to get lock in barrier.\n");
#line 340
		exit(-1);
#line 340
	}
#line 340

#line 340
	Cycle = (bars->barrier).cycle;
#line 340
	if (++(bars->barrier).counter != (nprocs)) {
#line 340
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 340
		while (Cycle == (bars->barrier).cycle) {
#line 340
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 340
			if (Error != 0) {
#line 340
				break;
#line 340
			}
#line 340
		}
#line 340
		pthread_setcancelstate(Cancel, &Temp);
#line 340
	} else {
#line 340
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 340
		(bars->barrier).counter = 0;
#line 340
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 340
	}
#line 340
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 340
}
#endif
/* compute psib array (one-time computation) and integrate into psibi */

   istart = 1;
   iend = istart + gp[procid].rel_num_y[numlev-1] - 1;
   jstart = 1;
   jend = jstart + gp[procid].rel_num_x[numlev-1] - 1;
   ist = istart;
   ien = iend;
   jst = jstart;
   jen = jend;

   if (gp[procid].neighbors[UP] == -1) {
     istart = 0;
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     jstart = 0;
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     iend = im-1;
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     jend = jm-1;
   }

   t2a = (double **) rhs_multi[procid][numlev-1];
   t2b = (double **) psib[procid];
   for(i=istart;i<=iend;i++) {
     t1a = (double *) t2a[i];
     t1b = (double *) t2b[i];
     for(j=jstart;j<=jend;j++) {
       t1a[j] = t1b[j] * ressqr;
     }
   }
   t2a = (double **) q_multi[procid][numlev-1];
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     t1b = (double *) t2b[0];
     for(j=jstart;j<=jend;j++) {
       t1a[j] = t1b[j];
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     t1b = (double *) t2b[im-1];
     for(j=jstart;j<=jend;j++) {
       t1a[j] = t1b[j];
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(i=istart;i<=iend;i++) {
       t2a[i][0] = t2b[i][0];
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     for(i=istart;i<=iend;i++) {
       t2a[i][jm-1] = t2b[i][jm-1];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 401
	unsigned long	Error, Cycle;
#line 401
	long		Cancel, Temp;
#line 401

#line 401
	Error = pthread_mutex_lock(&(bars->sl_psini).mutex);
#line 401
	if (Error != 0) {
#line 401
		printf("Error while trying to get lock in barrier.\n");
#line 401
		exit(-1);
#line 401
	}
#line 401

#line 401
	Cycle = (bars->sl_psini).cycle;
#line 401
	if (++(bars->sl_psini).counter != (nprocs)) {
#line 401
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 401
		while (Cycle == (bars->sl_psini).cycle) {
#line 401
			Error = pthread_cond_wait(&(bars->sl_psini).cv, &(bars->sl_psini).mutex);
#line 401
			if (Error != 0) {
#line 401
				break;
#line 401
			}
#line 401
		}
#line 401
		pthread_setcancelstate(Cancel, &Temp);
#line 401
	} else {
#line 401
		(bars->sl_psini).cycle = !(bars->sl_psini).cycle;
#line 401
		(bars->sl_psini).counter = 0;
#line 401
		Error = pthread_cond_broadcast(&(bars->sl_psini).cv);
#line 401
	}
#line 401
	pthread_mutex_unlock(&(bars->sl_psini).mutex);
#line 401
}
#else
   {
#line 403
	unsigned long	Error, Cycle;
#line 403
	long		Cancel, Temp;
#line 403

#line 403
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 403
	if (Error != 0) {
#line 403
		printf("Error while trying to get lock in barrier.\n");
#line 403
		exit(-1);
#line 403
	}
#line 403

#line 403
	Cycle = (bars->barrier).cycle;
#line 403
	if (++(bars->barrier).counter != (nprocs)) {
#line 403
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 403
		while (Cycle == (bars->barrier).cycle) {
#line 403
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 403
			if (Error != 0) {
#line 403
				break;
#line 403
			}
#line 403
		}
#line 403
		pthread_setcancelstate(Cancel, &Temp);
#line 403
	} else {
#line 403
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 403
		(bars->barrier).counter = 0;
#line 403
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 403
	}
#line 403
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 403
}
#endif
   t2a = (double **) psib[procid];
   j = gp[procid].neighbors[UP];
   if (j != -1) {
     t1a = (double *) t2a[0];
     t1b = (double *) psib[j][im-2];
     for (i=1;i<jm-1;i++) {
       t1a[i] = t1b[i];
     }
   }
   j = gp[procid].neighbors[DOWN];
   if (j != -1) {
     t1a = (double *) t2a[im-1];
     t1b = (double *) psib[j][1];
     for (i=1;i<jm-1;i++) {
       t1a[i] = t1b[i];
     }
   }
   j = gp[procid].neighbors[LEFT];
   if (j != -1) {
     t2b = (double **) psib[j];
     for (i=1;i<im-1;i++) {
       t2a[i][0] = t2b[i][jm-2];
     }
   }
   j = gp[procid].neighbors[RIGHT];
   if (j != -1) {
     t2b = (double **) psib[j];
     for (i=1;i<im-1;i++) {
       t2a[i][jm-1] = t2b[i][1];
     }
   }

   t2a = (double **) q_multi[procid][numlev-1];
   t2b = (double **) psib[procid];
   fac = 1.0 / (4.0 - ressqr*eig2);
   for(i=ist;i<=ien;i++) {
     t1a = (double *) t2a[i];
     t1b = (double *) t2b[i];
     t1c = (double *) t2b[i-1];
     t1d = (double *) t2b[i+1];
     for(j=jst;j<=jen;j++) {
       t1a[j] = fac * (t1d[j]+t1c[j]+t1b[j+1]+t1b[j-1] -
                   ressqr*t1b[j]);
     }
   }

   multig(procid);

   for(i=istart;i<=iend;i++) {
     t1a = (double *) t2a[i];
     t1b = (double *) t2b[i];
     for(j=jstart;j<=jend;j++) {
       t1b[j] = t1a[j];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 461
	unsigned long	Error, Cycle;
#line 461
	long		Cancel, Temp;
#line 461

#line 461
	Error = pthread_mutex_lock(&(bars->sl_prini).mutex);
#line 461
	if (Error != 0) {
#line 461
		printf("Error while trying to get lock in barrier.\n");
#line 461
		exit(-1);
#line 461
	}
#line 461

#line 461
	Cycle = (bars->sl_prini).cycle;
#line 461
	if (++(bars->sl_prini).counter != (nprocs)) {
#line 461
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 461
		while (Cycle == (bars->sl_prini).cycle) {
#line 461
			Error = pthread_cond_wait(&(bars->sl_prini).cv, &(bars->sl_prini).mutex);
#line 461
			if (Error != 0) {
#line 461
				break;
#line 461
			}
#line 461
		}
#line 461
		pthread_setcancelstate(Cancel, &Temp);
#line 461
	} else {
#line 461
		(bars->sl_prini).cycle = !(bars->sl_prini).cycle;
#line 461
		(bars->sl_prini).counter = 0;
#line 461
		Error = pthread_cond_broadcast(&(bars->sl_prini).cv);
#line 461
	}
#line 461
	pthread_mutex_unlock(&(bars->sl_prini).mutex);
#line 461
}
#else
   {
#line 463
	unsigned long	Error, Cycle;
#line 463
	long		Cancel, Temp;
#line 463

#line 463
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 463
	if (Error != 0) {
#line 463
		printf("Error while trying to get lock in barrier.\n");
#line 463
		exit(-1);
#line 463
	}
#line 463

#line 463
	Cycle = (bars->barrier).cycle;
#line 463
	if (++(bars->barrier).counter != (nprocs)) {
#line 463
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 463
		while (Cycle == (bars->barrier).cycle) {
#line 463
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 463
			if (Error != 0) {
#line 463
				break;
#line 463
			}
#line 463
		}
#line 463
		pthread_setcancelstate(Cancel, &Temp);
#line 463
	} else {
#line 463
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 463
		(bars->barrier).counter = 0;
#line 463
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 463
	}
#line 463
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 463
}
#endif
/* update the local running sum psibipriv by summing all the resulting
   values in that process's share of the psib matrix   */

   t2a = (double **) psib[procid];
   psibipriv=0.0;
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     psibipriv = psibipriv + 0.25*(t2a[0][0]);
   }
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     psibipriv = psibipriv + 0.25*(t2a[0][jm-1]);
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     psibipriv=psibipriv+0.25*(t2a[im-1][0]);
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     psibipriv=psibipriv+0.25*(t2a[im-1][jm-1]);
   }
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     for(j=firstcol;j<=lastcol;j++) {
       psibipriv = psibipriv + 0.5*t1a[j];
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     for(j=firstcol;j<=lastcol;j++) {
       psibipriv = psibipriv + 0.5*t1a[j];
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       psibipriv = psibipriv + 0.5*t2a[j][0];
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       psibipriv = psibipriv + 0.5*t2a[j][jm-1];
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     t1a = (double *) t2a[i];
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       psibipriv = psibipriv + t1a[iindex];
     }
   }

/* update the shared variable psibi by summing all the psibiprivs
   of the individual processes into it.  note that this combined
   private and shared sum method avoids accessing the shared
   variable psibi once for every element of the matrix.  */

   {pthread_mutex_lock(&(locks->psibilock));}
     global->psibi = global->psibi + psibipriv;
   {pthread_mutex_unlock(&(locks->psibilock));}

/* initialize psim matrices

   if there is more than one process, then split the processes
   between the two psim matrices; otherwise, let the single process
   work on one first and then the other   */

   for(psiindex=0;psiindex<=1;psiindex++) {
     t2a = (double **) psim[procid][psiindex];
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[0][0] = 0.0;
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[im-1][0] = 0.0;
     }
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[0][jm-1] = 0.0;
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[im-1][jm-1] = 0.0;
     }
     if (gp[procid].neighbors[UP] == -1) {
       t1a = (double *) t2a[0];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = 0.0;
       }
     }
     if (gp[procid].neighbors[DOWN] == -1) {
       t1a = (double *) t2a[im-1];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = 0.0;
       }
     }
     if (gp[procid].neighbors[LEFT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][0] = 0.0;
       }
     }
     if (gp[procid].neighbors[RIGHT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][jm-1] = 0.0;
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       t1a = (double *) t2a[i];
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         t1a[iindex] = 0.0;
       }
     }
   }

/* initialize psi matrices the same way  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     t2a = (double **) psi[procid][psiindex];
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[0][0] = 0.0;
     }
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[0][jm-1] = 0.0;
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[im-1][0] = 0.0;
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[im-1][jm-1] = 0.0;
     }
     if (gp[procid].neighbors[UP] == -1) {
       t1a = (double *) t2a[0];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = 0.0;
       }
     }
     if (gp[procid].neighbors[DOWN] == -1) {
       t1a = (double *) t2a[im-1];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = 0.0;
       }
     }
     if (gp[procid].neighbors[LEFT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][0] = 0.0;
       }
     }
     if (gp[procid].neighbors[RIGHT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][jm-1] = 0.0;
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       t1a = (double *) t2a[i];
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         t1a[iindex] = 0.0;
       }
     }
   }

/* compute input curl of wind stress */

   t2a = (double **) tauz[procid];
   ysca1 = .5*ysca;
   factor= -t0*pi/ysca1;
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[0][0] = 0.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[im-1][0] = 0.0;
   }
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     sintemp = pi*((double) jm-1+j_off)*res/ysca1;
     sintemp = sin(sintemp);
     t2a[0][jm-1] = factor*sintemp;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     sintemp = pi*((double) jm-1+j_off)*res/ysca1;
     sintemp = sin(sintemp);
     t2a[im-1][jm-1] = factor*sintemp;
   }
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     for(j=firstcol;j<=lastcol;j++) {
       sintemp = pi*((double) j+j_off)*res/ysca1;
       sintemp = sin(sintemp);
       curlt = factor*sintemp;
       t1a[j] = curlt;
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     for(j=firstcol;j<=lastcol;j++) {
       sintemp = pi*((double) j+j_off)*res/ysca1;
       sintemp = sin(sintemp);
       curlt = factor*sintemp;
       t1a[j] = curlt;
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][0] = 0.0;
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     sintemp = pi*((double) jm-1+j_off)*res/ysca1;
     sintemp = sin(sintemp);
     curlt = factor*sintemp;
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][jm-1] = curlt;
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     t1a = (double *) t2a[i];
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       sintemp = pi*((double) iindex+j_off)*res/ysca1;
       sintemp = sin(sintemp);
       curlt = factor*sintemp;
       t1a[iindex] = curlt;
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 678
	unsigned long	Error, Cycle;
#line 678
	long		Cancel, Temp;
#line 678

#line 678
	Error = pthread_mutex_lock(&(bars->sl_onetime).mutex);
#line 678
	if (Error != 0) {
#line 678
		printf("Error while trying to get lock in barrier.\n");
#line 678
		exit(-1);
#line 678
	}
#line 678

#line 678
	Cycle = (bars->sl_onetime).cycle;
#line 678
	if (++(bars->sl_onetime).counter != (nprocs)) {
#line 678
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 678
		while (Cycle == (bars->sl_onetime).cycle) {
#line 678
			Error = pthread_cond_wait(&(bars->sl_onetime).cv, &(bars->sl_onetime).mutex);
#line 678
			if (Error != 0) {
#line 678
				break;
#line 678
			}
#line 678
		}
#line 678
		pthread_setcancelstate(Cancel, &Temp);
#line 678
	} else {
#line 678
		(bars->sl_onetime).cycle = !(bars->sl_onetime).cycle;
#line 678
		(bars->sl_onetime).counter = 0;
#line 678
		Error = pthread_cond_broadcast(&(bars->sl_onetime).cv);
#line 678
	}
#line 678
	pthread_mutex_unlock(&(bars->sl_onetime).mutex);
#line 678
}
#else
   {
#line 680
	unsigned long	Error, Cycle;
#line 680
	long		Cancel, Temp;
#line 680

#line 680
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 680
	if (Error != 0) {
#line 680
		printf("Error while trying to get lock in barrier.\n");
#line 680
		exit(-1);
#line 680
	}
#line 680

#line 680
	Cycle = (bars->barrier).cycle;
#line 680
	if (++(bars->barrier).counter != (nprocs)) {
#line 680
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 680
		while (Cycle == (bars->barrier).cycle) {
#line 680
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 680
			if (Error != 0) {
#line 680
				break;
#line 680
			}
#line 680
		}
#line 680
		pthread_setcancelstate(Cancel, &Temp);
#line 680
	} else {
#line 680
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 680
		(bars->barrier).counter = 0;
#line 680
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 680
	}
#line 680
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 680
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
#line 693
	struct timeval	FullTime;
#line 693

#line 693
	gettimeofday(&FullTime, NULL);
#line 693
	(global->trackstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 693
}
         }
	 if ((procid == MASTER) || (do_stats)) {
	   {
#line 696
	struct timeval	FullTime;
#line 696

#line 696
	gettimeofday(&FullTime, NULL);
#line 696
	(t1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 696
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

     t2a = (double **) psium[procid];
     t2b = (double **) psim[procid][0];
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[0][0] = t2a[0][0]+t2b[0][0];
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[im-1][0] = t2a[im-1][0]+t2b[im-1][0];
     }
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[0][jm-1] = t2a[0][jm-1]+t2b[0][jm-1];
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[im-1][jm-1] = t2a[im-1][jm-1] +
				   t2b[im-1][jm-1];
     }
     if (gp[procid].neighbors[UP] == -1) {
       t1a = (double *) t2a[0];
       t1b = (double *) t2b[0];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = t1a[j]+t1b[j];
       }
     }
     if (gp[procid].neighbors[DOWN] == -1) {
       t1a = (double *) t2a[im-1];
       t1b = (double *) t2b[im-1];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = t1a[j] + t1b[j];
       }
     }
     if (gp[procid].neighbors[LEFT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][0] = t2a[j][0]+t2b[j][0];
       }
     }
     if (gp[procid].neighbors[RIGHT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][jm-1] = t2a[j][jm-1] +
				  t2b[j][jm-1];
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       t1a = (double *) t2a[i];
       t1b = (double *) t2b[i];
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         t1a[iindex] = t1a[iindex] + t1b[iindex];
       }
     }

/* update values of psilm array to psilm + psim[2]  */

     t2a = (double **) psilm[procid];
     t2b = (double **) psim[procid][1];
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[0][0] = t2a[0][0]+t2b[0][0];
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[im-1][0] = t2a[im-1][0]+t2b[im-1][0];
     }
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[0][jm-1] = t2a[0][jm-1]+t2b[0][jm-1];
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[im-1][jm-1] = t2a[im-1][jm-1] +
				   t2b[im-1][jm-1];
     }
     if (gp[procid].neighbors[UP] == -1) {
       t1a = (double *) t2a[0];
       t1b = (double *) t2b[0];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = t1a[j]+t1b[j];
       }
     }
     if (gp[procid].neighbors[DOWN] == -1) {
       t1a = (double *) t2a[im-1];
       t1b = (double *) t2b[im-1];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = t1a[j]+t1b[j];
       }
     }
     if (gp[procid].neighbors[LEFT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][0] = t2a[j][0]+t2b[j][0];
       }
     }
     if (gp[procid].neighbors[RIGHT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][jm-1] = t2a[j][jm-1] + t2b[j][jm-1];
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       t1a = (double *) t2a[i];
       t1b = (double *) t2b[i];
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         t1a[iindex] = t1a[iindex] + t1b[iindex];
       }
     }
     if (iday >= (long) outday3) {
       endflag = 1;
     }
  }
  if ((procid == MASTER) || (do_stats)) {
    {
#line 826
	struct timeval	FullTime;
#line 826

#line 826
	gettimeofday(&FullTime, NULL);
#line 826
	(t1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 826
};
    gp[procid].total_time = t1-gp[procid].total_time;
  }
}

