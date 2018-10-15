#line 228 "../null_macros/c.m4.null"

#line 1 "slave2.C"
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
      subroutine slave2
      ****************  */

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "decs.h"

void slave2(long procid, long firstrow, long lastrow, long numrows, long firstcol, long lastcol, long numcols)
{
   long i;
   long j;
   long iindex;
   double hh1;
   double hh3;
   double hinv;
   double h1inv;
   long istart;
   long iend;
   long jstart;
   long jend;
   long ist;
   long ien;
   long jst;
   long jen;
   double fac;
   double ressqr;
   double timst;
   double f4;
   long psiindex;
   double psiaipriv;
   long multi_start;
   long multi_end;

   ressqr = lev_res[numlev-1] * lev_res[numlev-1];

/*   ***************************************************************

          f i r s t     p h a s e   (of timestep calculation)

     ***************************************************************/

   if (procid == MASTER) {
     wrk1->ga[0][0]=0.0;
   }
   if (procid == nprocs-xprocs) {
     wrk1->ga[im-1][0]=0.0;
   }
   if (procid == xprocs-1) {
     wrk1->ga[0][jm-1]=0.0;
   }
   if (procid == nprocs-1) {
     wrk1->ga[im-1][jm-1]=0.0;
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk1->ga[0][j] = 0.0;
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk1->ga[im-1][j] = 0.0;
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk1->ga[j][0] = 0.0;
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk1->ga[j][jm-1] = 0.0;
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
         wrk1->ga[i][iindex] = 0.0;
     }
   }

   if (procid == MASTER) {
     wrk1->gb[0][0]=0.0;
   }
   if (procid == nprocs-xprocs) {
     wrk1->gb[im-1][0]=0.0;
   }
   if (procid == xprocs-1) {
     wrk1->gb[0][jm-1]=0.0;
   }
   if (procid == nprocs-1) {
     wrk1->gb[im-1][jm-1]=0.0;
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk1->gb[0][j] = 0.0;
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk1->gb[im-1][j] = 0.0;
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk1->gb[j][0] = 0.0;
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk1->gb[j][jm-1] = 0.0;
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       wrk1->gb[i][iindex] = 0.0;
     }
   }

/* put the laplacian of psi{1,3} in work1{1,2}
   note that psi(i,j,2) represents the psi3 array in
   the original equations  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     if (procid == MASTER) {
       wrk3->work1[psiindex][0][0] = 0;
     }
     if (procid == nprocs-xprocs) {
       wrk3->work1[psiindex][im-1][0] = 0;
     }
     if (procid == xprocs-1) {
       wrk3->work1[psiindex][0][jm-1] = 0;
     }
     if (procid == nprocs-1) {
       wrk3->work1[psiindex][im-1][jm-1] = 0;
     }
     laplacalc(fields->psi[psiindex],
	       wrk3->work1[psiindex],
	       firstrow,lastrow,firstcol,lastcol,numrows,numcols);

   }


   if (procid == MASTER) {
     wrk3->work2[0][0] = fields->psi[0][0][0]-fields->psi[1][0][0];
   }
   if (procid == nprocs-xprocs) {
     wrk3->work2[im-1][0] = fields->psi[0][im-1][0]-fields->psi[1][im-1][0];
   }
   if (procid == xprocs-1) {
     wrk3->work2[0][jm-1] = fields->psi[0][0][jm-1]-fields->psi[1][0][jm-1];
   }
   if (procid == nprocs-1) {
     wrk3->work2[im-1][jm-1] = fields->psi[0][im-1][jm-1]-fields->psi[1][im-1][jm-1];
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk3->work2[0][j] = fields->psi[0][0][j]-fields->psi[1][0][j];
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk3->work2[im-1][j] = fields->psi[0][im-1][j]-fields->psi[1][im-1][j];
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk3->work2[j][0] = fields->psi[0][j][0]-fields->psi[1][j][0];
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk3->work2[j][jm-1] = fields->psi[0][j][jm-1]-fields->psi[1][j][jm-1];
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
         wrk3->work2[i][iindex] = fields->psi[0][i][iindex]-fields->psi[1][i][iindex];
     }
   }

/* set values of work3 array to h3/h * psi1 + h1/h * psi3  */

   hh3 = h3/h;
   hh1 = h1/h;
   if (procid == MASTER) {
     wrk2->work3[0][0] = hh3*fields->psi[0][0][0]+hh1*fields->psi[1][0][0];
   }
   if (procid == nprocs-xprocs) {
     wrk2->work3[im-1][0] = hh3*fields->psi[0][im-1][0]+hh1*fields->psi[1][im-1][0];
   }
   if (procid == xprocs-1) {
     wrk2->work3[0][jm-1] = hh3*fields->psi[0][0][jm-1]+hh1*fields->psi[1][0][jm-1];
   }
   if (procid == nprocs-1) {
     wrk2->work3[im-1][jm-1] = hh3*fields->psi[0][im-1][jm-1]+hh1*fields->psi[1][im-1][jm-1];
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk2->work3[0][j] = hh3*fields->psi[0][0][j]+hh1*fields->psi[1][0][j];
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk2->work3[im-1][j] = hh3*fields->psi[0][im-1][j]+hh1*fields->psi[1][im-1][j];
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk2->work3[j][0] = hh3*fields->psi[0][j][0]+hh1*fields->psi[1][j][0];
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk2->work3[j][jm-1] = hh3*fields->psi[0][j][jm-1]+hh1*fields->psi[1][j][jm-1];
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
        wrk2->work3[i][iindex] = hh3*fields->psi[0][i][iindex]+hh1*fields->psi[1][i][iindex];
     }
   }

/* set values of temparray{1,3} to psim{1,3}  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     if (procid == MASTER) {
       wrk5->temparray[psiindex][0][0] = fields->psi[psiindex][0][0];
     }
     if (procid == nprocs-xprocs) {
       wrk5->temparray[psiindex][im-1][0] = fields->psi[psiindex][im-1][0];
     }
     if (procid == xprocs-1) {
       wrk5->temparray[psiindex][0][jm-1] = fields->psi[psiindex][0][jm-1];
     }
     if (procid == nprocs-1) {
       wrk5->temparray[psiindex][im-1][jm-1] = fields->psi[psiindex][im-1][jm-1];
     }
     if (firstrow == 1) {
       for(j=firstcol;j<=lastcol;j++) {
         wrk5->temparray[psiindex][0][j] = fields->psi[psiindex][0][j];
       }
     }
     if ((firstrow+numrows) == im-1) {
       for(j=firstcol;j<=lastcol;j++) {
         wrk5->temparray[psiindex][im-1][j] = fields->psi[psiindex][im-1][j];
       }
     }
     if (firstcol == 1) {
       for(j=firstrow;j<=lastrow;j++) {
         wrk5->temparray[psiindex][j][0] = fields->psi[psiindex][j][0];
       }
     }
     if ((firstcol+numcols) == jm-1) {
       for(j=firstrow;j<=lastrow;j++) {
         wrk5->temparray[psiindex][j][jm-1] = fields->psi[psiindex][j][jm-1];
       }
     }

     for(i=firstrow;i<=lastrow;i++) {
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         wrk5->temparray[psiindex][i][iindex] = fields->psi[psiindex][i][iindex];
       }
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 284
	unsigned long	Error, Cycle;
#line 284
	long		Cancel, Temp;
#line 284

#line 284
	Error = pthread_mutex_lock(&(bars->sl_phase_1).mutex);
#line 284
	if (Error != 0) {
#line 284
		printf("Error while trying to get lock in barrier.\n");
#line 284
		exit(-1);
#line 284
	}
#line 284

#line 284
	Cycle = (bars->sl_phase_1).cycle;
#line 284
	if (++(bars->sl_phase_1).counter != (nprocs)) {
#line 284
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 284
		while (Cycle == (bars->sl_phase_1).cycle) {
#line 284
			Error = pthread_cond_wait(&(bars->sl_phase_1).cv, &(bars->sl_phase_1).mutex);
#line 284
			if (Error != 0) {
#line 284
				break;
#line 284
			}
#line 284
		}
#line 284
		pthread_setcancelstate(Cancel, &Temp);
#line 284
	} else {
#line 284
		(bars->sl_phase_1).cycle = !(bars->sl_phase_1).cycle;
#line 284
		(bars->sl_phase_1).counter = 0;
#line 284
		Error = pthread_cond_broadcast(&(bars->sl_phase_1).cv);
#line 284
	}
#line 284
	pthread_mutex_unlock(&(bars->sl_phase_1).mutex);
#line 284
}
#else
   {
#line 286
	unsigned long	Error, Cycle;
#line 286
	long		Cancel, Temp;
#line 286

#line 286
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 286
	if (Error != 0) {
#line 286
		printf("Error while trying to get lock in barrier.\n");
#line 286
		exit(-1);
#line 286
	}
#line 286

#line 286
	Cycle = (bars->barrier).cycle;
#line 286
	if (++(bars->barrier).counter != (nprocs)) {
#line 286
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 286
		while (Cycle == (bars->barrier).cycle) {
#line 286
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 286
			if (Error != 0) {
#line 286
				break;
#line 286
			}
#line 286
		}
#line 286
		pthread_setcancelstate(Cancel, &Temp);
#line 286
	} else {
#line 286
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 286
		(bars->barrier).counter = 0;
#line 286
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 286
	}
#line 286
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 286
}
#endif
/*     *******************************************************

              s e c o n d   p h a s e

       *******************************************************

   set values of psi{1,3} to psim{1,3}   */

   for(psiindex=0;psiindex<=1;psiindex++) {
     if (procid == MASTER) {
       fields->psi[psiindex][0][0] = fields->psim[psiindex][0][0];
     }
     if (procid == xprocs-1) {
       fields->psi[psiindex][0][jm-1] = fields->psim[psiindex][0][jm-1];
     }
     if (procid == nprocs-xprocs) {
       fields->psi[psiindex][im-1][0] = fields->psim[psiindex][im-1][0];
     }
     if (procid == nprocs-1) {
       fields->psi[psiindex][im-1][jm-1] = fields->psim[psiindex][im-1][jm-1];
     }
     if (firstrow == 1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields->psi[psiindex][0][j] = fields->psim[psiindex][0][j];
       }
     }
     if ((firstrow+numrows) == im-1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields->psi[psiindex][im-1][j] = fields->psim[psiindex][im-1][j];
       }
     }
     if (firstcol == 1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields->psi[psiindex][j][0] = fields->psim[psiindex][j][0];
       }
     }
     if ((firstcol+numcols) == jm-1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields->psi[psiindex][j][jm-1] = fields->psim[psiindex][j][jm-1];
       }
     }

     for(i=firstrow;i<=lastrow;i++) {
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         fields->psi[psiindex][i][iindex] = fields->psim[psiindex][i][iindex];
       }
     }
   }

/* put the laplacian of the psim array
   into the work7 array; first part of a three-laplacian
   calculation to compute the friction terms  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     if (procid == MASTER) {
       wrk5->work7[psiindex][0][0] = 0;
     }
     if (procid == nprocs-xprocs) {
       wrk5->work7[psiindex][im-1][0] = 0;
     }
     if (procid == xprocs-1) {
       wrk5->work7[psiindex][0][jm-1] = 0;
     }
     if (procid == nprocs-1) {
       wrk5->work7[psiindex][im-1][jm-1] = 0;
     }
     laplacalc(fields->psim[psiindex],wrk5->work7[psiindex],firstrow,lastrow,firstcol,lastcol,numrows,numcols);
   }

/* to the values of the work1{1,2} arrays obtained from the
   laplacians of psi{1,2} in the previous phase, add to the
   elements of every column the corresponding value in the
   one-dimenional f array  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     if (procid == MASTER) {
       wrk3->work1[psiindex][0][0] = wrk3->work1[psiindex][0][0] + wrk2->f[0];
     }
     if (procid == nprocs-xprocs) {
       wrk3->work1[psiindex][im-1][0] = wrk3->work1[psiindex][im-1][0] + wrk2->f[0];
     }
     if (procid == xprocs-1) {
       wrk3->work1[psiindex][0][jm-1] = wrk3->work1[psiindex][0][jm-1] + wrk2->f[jm-1];
     }
     if (procid == nprocs-1) {
       wrk3->work1[psiindex][im-1][jm-1] = wrk3->work1[psiindex][im-1][jm-1] + wrk2->f[jm-1];
     }
     if (firstrow == 1) {
       for(j=firstcol;j<=lastcol;j++) {
         wrk3->work1[psiindex][0][j] = wrk3->work1[psiindex][0][j] + wrk2->f[j];
       }
     }
     if ((firstrow+numrows) == im-1) {
       for(j=firstcol;j<=lastcol;j++) {
         wrk3->work1[psiindex][im-1][j] = wrk3->work1[psiindex][im-1][j] + wrk2->f[j];
       }
     }
     if (firstcol == 1) {
       for(j=firstrow;j<=lastrow;j++) {
         wrk3->work1[psiindex][j][0] = wrk3->work1[psiindex][j][0] + wrk2->f[j];
       }
     }
     if ((firstcol+numcols) == jm-1) {
       for(j=firstrow;j<=lastrow;j++) {
         wrk3->work1[psiindex][j][jm-1] = wrk3->work1[psiindex][j][jm-1] + wrk2->f[j];
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         wrk3->work1[psiindex][i][iindex] = wrk3->work1[psiindex][i][iindex] +
					   wrk2->f[iindex];
       }
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 403
	unsigned long	Error, Cycle;
#line 403
	long		Cancel, Temp;
#line 403

#line 403
	Error = pthread_mutex_lock(&(bars->sl_phase_2).mutex);
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
	Cycle = (bars->sl_phase_2).cycle;
#line 403
	if (++(bars->sl_phase_2).counter != (nprocs)) {
#line 403
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 403
		while (Cycle == (bars->sl_phase_2).cycle) {
#line 403
			Error = pthread_cond_wait(&(bars->sl_phase_2).cv, &(bars->sl_phase_2).mutex);
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
		(bars->sl_phase_2).cycle = !(bars->sl_phase_2).cycle;
#line 403
		(bars->sl_phase_2).counter = 0;
#line 403
		Error = pthread_cond_broadcast(&(bars->sl_phase_2).cv);
#line 403
	}
#line 403
	pthread_mutex_unlock(&(bars->sl_phase_2).mutex);
#line 403
}
#else
   {
#line 405
	unsigned long	Error, Cycle;
#line 405
	long		Cancel, Temp;
#line 405

#line 405
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 405
	if (Error != 0) {
#line 405
		printf("Error while trying to get lock in barrier.\n");
#line 405
		exit(-1);
#line 405
	}
#line 405

#line 405
	Cycle = (bars->barrier).cycle;
#line 405
	if (++(bars->barrier).counter != (nprocs)) {
#line 405
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 405
		while (Cycle == (bars->barrier).cycle) {
#line 405
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 405
			if (Error != 0) {
#line 405
				break;
#line 405
			}
#line 405
		}
#line 405
		pthread_setcancelstate(Cancel, &Temp);
#line 405
	} else {
#line 405
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 405
		(bars->barrier).counter = 0;
#line 405
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 405
	}
#line 405
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 405
}
#endif
/* 	*******************************************************

                 t h i r d   p h a s e

 	*******************************************************

   put the jacobian of the work1{1,2} and psi{1,3} arrays
   (the latter currently in temparray) in the work5{1,2} arrays  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     jacobcalc(wrk3->work1[psiindex],wrk5->temparray[psiindex],
               wrk4->work5[psiindex],procid,firstrow,lastrow,firstcol,lastcol,numrows,numcols);
   }


/* set values of psim{1,3} to temparray{1,3}  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     if (procid == MASTER) {
       fields->psim[psiindex][0][0] = wrk5->temparray[psiindex][0][0];
     }
     if (procid == nprocs-xprocs) {
       fields->psim[psiindex][im-1][0] = wrk5->temparray[psiindex][im-1][0];
     }
     if (procid == xprocs-1) {
       fields->psim[psiindex][0][jm-1] = wrk5->temparray[psiindex][0][jm-1];
     }
     if (procid == nprocs-1) {
       fields->psim[psiindex][im-1][jm-1] = wrk5->temparray[psiindex][im-1][jm-1];
     }
     if (firstrow == 1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields->psim[psiindex][0][j] = wrk5->temparray[psiindex][0][j];
       }
     }
     if ((firstrow+numrows) == im-1) {
       for(j=firstcol;j<=lastcol;j++) {
         fields->psim[psiindex][im-1][j] = wrk5->temparray[psiindex][im-1][j];
       }
     }
     if (firstcol == 1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields->psim[psiindex][j][0] = wrk5->temparray[psiindex][j][0];
       }
     }
     if ((firstcol+numcols) == jm-1) {
       for(j=firstrow;j<=lastrow;j++) {
         fields->psim[psiindex][j][jm-1] = wrk5->temparray[psiindex][j][jm-1];
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         fields->psim[psiindex][i][iindex] = wrk5->temparray[psiindex][i][iindex];
       }
     }
   }

/* put the laplacian of the work7{1,2} arrays in the work4{1,2}
   arrays; second step in the three-laplacian friction calculation  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     laplacalc(wrk5->work7[psiindex],
	       wrk4->work4[psiindex],
               firstrow,lastrow,firstcol,lastcol,numrows,numcols);
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 473
	unsigned long	Error, Cycle;
#line 473
	long		Cancel, Temp;
#line 473

#line 473
	Error = pthread_mutex_lock(&(bars->sl_phase_3).mutex);
#line 473
	if (Error != 0) {
#line 473
		printf("Error while trying to get lock in barrier.\n");
#line 473
		exit(-1);
#line 473
	}
#line 473

#line 473
	Cycle = (bars->sl_phase_3).cycle;
#line 473
	if (++(bars->sl_phase_3).counter != (nprocs)) {
#line 473
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 473
		while (Cycle == (bars->sl_phase_3).cycle) {
#line 473
			Error = pthread_cond_wait(&(bars->sl_phase_3).cv, &(bars->sl_phase_3).mutex);
#line 473
			if (Error != 0) {
#line 473
				break;
#line 473
			}
#line 473
		}
#line 473
		pthread_setcancelstate(Cancel, &Temp);
#line 473
	} else {
#line 473
		(bars->sl_phase_3).cycle = !(bars->sl_phase_3).cycle;
#line 473
		(bars->sl_phase_3).counter = 0;
#line 473
		Error = pthread_cond_broadcast(&(bars->sl_phase_3).cv);
#line 473
	}
#line 473
	pthread_mutex_unlock(&(bars->sl_phase_3).mutex);
#line 473
}
#else
   {
#line 475
	unsigned long	Error, Cycle;
#line 475
	long		Cancel, Temp;
#line 475

#line 475
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 475
	if (Error != 0) {
#line 475
		printf("Error while trying to get lock in barrier.\n");
#line 475
		exit(-1);
#line 475
	}
#line 475

#line 475
	Cycle = (bars->barrier).cycle;
#line 475
	if (++(bars->barrier).counter != (nprocs)) {
#line 475
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 475
		while (Cycle == (bars->barrier).cycle) {
#line 475
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 475
			if (Error != 0) {
#line 475
				break;
#line 475
			}
#line 475
		}
#line 475
		pthread_setcancelstate(Cancel, &Temp);
#line 475
	} else {
#line 475
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 475
		(bars->barrier).counter = 0;
#line 475
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 475
	}
#line 475
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 475
}
#endif
/*     *******************************************************

                f o u r t h   p h a s e

       *******************************************************

   put the jacobian of the work2 and work3 arrays in the work6
   array  */

   jacobcalc(wrk3->work2,wrk2->work3,wrk6->work6,procid,firstrow,
             lastrow,firstcol,lastcol,numrows,numcols);

/* put the laplacian of the work4{1,2} arrays in the work7{1,2}
   arrays; third step in the three-laplacian friction calculation  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     laplacalc(wrk4->work4[psiindex],
               wrk5->work7[psiindex],
               firstrow,lastrow,firstcol,lastcol,numrows,numcols);
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 498
	unsigned long	Error, Cycle;
#line 498
	long		Cancel, Temp;
#line 498

#line 498
	Error = pthread_mutex_lock(&(bars->sl_phase_4).mutex);
#line 498
	if (Error != 0) {
#line 498
		printf("Error while trying to get lock in barrier.\n");
#line 498
		exit(-1);
#line 498
	}
#line 498

#line 498
	Cycle = (bars->sl_phase_4).cycle;
#line 498
	if (++(bars->sl_phase_4).counter != (nprocs)) {
#line 498
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 498
		while (Cycle == (bars->sl_phase_4).cycle) {
#line 498
			Error = pthread_cond_wait(&(bars->sl_phase_4).cv, &(bars->sl_phase_4).mutex);
#line 498
			if (Error != 0) {
#line 498
				break;
#line 498
			}
#line 498
		}
#line 498
		pthread_setcancelstate(Cancel, &Temp);
#line 498
	} else {
#line 498
		(bars->sl_phase_4).cycle = !(bars->sl_phase_4).cycle;
#line 498
		(bars->sl_phase_4).counter = 0;
#line 498
		Error = pthread_cond_broadcast(&(bars->sl_phase_4).cv);
#line 498
	}
#line 498
	pthread_mutex_unlock(&(bars->sl_phase_4).mutex);
#line 498
}
#else
   {
#line 500
	unsigned long	Error, Cycle;
#line 500
	long		Cancel, Temp;
#line 500

#line 500
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 500
	if (Error != 0) {
#line 500
		printf("Error while trying to get lock in barrier.\n");
#line 500
		exit(-1);
#line 500
	}
#line 500

#line 500
	Cycle = (bars->barrier).cycle;
#line 500
	if (++(bars->barrier).counter != (nprocs)) {
#line 500
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 500
		while (Cycle == (bars->barrier).cycle) {
#line 500
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 500
			if (Error != 0) {
#line 500
				break;
#line 500
			}
#line 500
		}
#line 500
		pthread_setcancelstate(Cancel, &Temp);
#line 500
	} else {
#line 500
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 500
		(bars->barrier).counter = 0;
#line 500
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 500
	}
#line 500
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 500
}
#endif
/*     *******************************************************

                f i f t h   p h a s e

       *******************************************************

   use the values of the work5, work6 and work7 arrays
   computed in the previous time-steps to compute the
   ga and gb arrays   */

   hinv = 1.0/h;
   h1inv = 1.0/h1;

   if (procid == MASTER) {
     wrk1->ga[0][0] = wrk4->work5[0][0][0]-wrk4->work5[1][0][0]+eig2*wrk6->work6[0][0]+h1inv*
                frcng->tauz[0][0]+lf*wrk5->work7[0][0][0]-lf*wrk5->work7[1][0][0];
     wrk1->gb[0][0] = hh1*wrk4->work5[0][0][0]+hh3*wrk4->work5[1][0][0]+hinv*frcng->tauz[0][0]+
                lf*hh1*wrk5->work7[0][0][0]+lf*hh3*wrk5->work7[1][0][0];
   }
   if (procid == nprocs-xprocs) {
     wrk1->ga[im-1][0] = wrk4->work5[0][im-1][0]-wrk4->work5[1][im-1][0]+eig2*wrk6->work6[im-1][0]+h1inv*
                   frcng->tauz[im-1][0]+lf*wrk5->work7[0][im-1][0]-lf*wrk5->work7[1][im-1][0];
     wrk1->gb[im-1][0] = hh1*wrk4->work5[0][im-1][0]+hh3*wrk4->work5[1][im-1][0]+hinv*frcng->tauz[im-1][0]+
                   lf*hh1*wrk5->work7[0][im-1][0]+lf*hh3*wrk5->work7[1][im-1][0];
   }
   if (procid == xprocs-1) {
     wrk1->ga[0][jm-1] = wrk4->work5[0][0][jm-1]-wrk4->work5[1][0][jm-1]+eig2*wrk6->work6[0][jm-1]+h1inv*
                   frcng->tauz[0][jm-1]+lf*wrk5->work7[0][0][jm-1]-lf*wrk5->work7[1][0][jm-1];
     wrk1->gb[0][jm-1] = hh1*wrk4->work5[0][0][jm-1]+hh3*wrk4->work5[1][0][jm-1]+hinv*frcng->tauz[0][jm-1]+
                   lf*hh1*wrk5->work7[0][0][jm-1]+lf*hh3*wrk5->work7[1][0][jm-1];
   }
   if (procid == nprocs-1) {
     wrk1->ga[im-1][jm-1] = wrk4->work5[0][im-1][jm-1]-wrk4->work5[1][im-1][jm-1]+eig2*wrk6->work6[im-1][jm-1]+
                      h1inv*frcng->tauz[im-1][jm-1]+lf*wrk5->work7[0][im-1][jm-1]-lf*wrk5->work7[1][im-1][jm-1];
     wrk1->gb[im-1][jm-1] = hh1*wrk4->work5[0][im-1][jm-1]+hh3*wrk4->work5[1][im-1][jm-1]+hinv*
		    frcng->tauz[im-1][jm-1]+lf*hh1*wrk5->work7[0][im-1][jm-1]+lf*hh3*wrk5->work7[1][im-1][jm-1];
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk1->ga[0][j] = wrk4->work5[0][0][j]-wrk4->work5[1][0][j]+eig2*
                    wrk6->work6[0][j]+h1inv*frcng->tauz[0][j]+lf*wrk5->work7[0][0][j]-lf*wrk5->work7[0][0][j];
       wrk1->gb[0][j] = hh1*wrk4->work5[0][0][j]+hh3*wrk4->work5[1][0][j]+hinv*
                    frcng->tauz[0][j]+lf*hh1*wrk5->work7[0][0][j]+lf*hh3*wrk5->work7[1][0][j];
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk1->ga[im-1][j] = wrk4->work5[0][im-1][j]-wrk4->work5[1][im-1][j]+eig2*
                       wrk6->work6[im-1][j]+h1inv*frcng->tauz[im-1][j]+
                       lf*wrk5->work7[0][im-1][j]-lf*wrk5->work7[1][im-1][j];
       wrk1->gb[im-1][j] = hh1*wrk4->work5[0][im-1][j]+hh3*wrk4->work5[1][im-1][j]+hinv*
                       frcng->tauz[im-1][j]+lf*hh1*wrk5->work7[0][im-1][j]+
                       lf*hh3*wrk5->work7[1][im-1][j];
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk1->ga[j][0] = wrk4->work5[0][j][0]-wrk4->work5[1][j][0]+eig2*
                  wrk6->work6[j][0]+h1inv*frcng->tauz[j][0]+lf*wrk5->work7[0][j][0]-lf*wrk5->work7[1][j][0];
       wrk1->gb[j][0] = hh1*wrk4->work5[0][j][0]+hh3*wrk4->work5[1][j][0]+hinv*
                  frcng->tauz[j][0]+lf*hh1*wrk5->work7[0][j][0]+lf*hh3*wrk5->work7[1][j][0];
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk1->ga[j][jm-1] = wrk4->work5[0][j][jm-1]-wrk4->work5[1][j][jm-1]+eig2*
                       wrk6->work6[j][jm-1]+h1inv*frcng->tauz[j][jm-1]+
                       lf*wrk5->work7[0][j][jm-1]-lf*wrk5->work7[1][j][jm-1];
       wrk1->gb[j][jm-1] = hh1*wrk4->work5[0][j][jm-1]+hh3*wrk4->work5[1][j][jm-1]+hinv*
                       frcng->tauz[j][jm-1]+lf*hh1*wrk5->work7[0][j][jm-1]+
                       lf*hh3*wrk5->work7[1][j][jm-1];
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       wrk1->ga[i][iindex] = wrk4->work5[0][i][iindex]-wrk4->work5[1][i][iindex]+eig2*
                          wrk6->work6[i][iindex]+h1inv*frcng->tauz[i][iindex]+
                          lf*wrk5->work7[0][i][iindex]-lf*wrk5->work7[1][i][iindex];
       wrk1->gb[i][iindex] = hh1*wrk4->work5[0][i][iindex]+hh3*wrk4->work5[1][i][iindex]+hinv*
                          frcng->tauz[i][iindex]+lf*hh1*wrk5->work7[0][i][iindex]+
                          lf*hh3*wrk5->work7[1][i][iindex];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 586
	unsigned long	Error, Cycle;
#line 586
	long		Cancel, Temp;
#line 586

#line 586
	Error = pthread_mutex_lock(&(bars->sl_phase_5).mutex);
#line 586
	if (Error != 0) {
#line 586
		printf("Error while trying to get lock in barrier.\n");
#line 586
		exit(-1);
#line 586
	}
#line 586

#line 586
	Cycle = (bars->sl_phase_5).cycle;
#line 586
	if (++(bars->sl_phase_5).counter != (nprocs)) {
#line 586
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 586
		while (Cycle == (bars->sl_phase_5).cycle) {
#line 586
			Error = pthread_cond_wait(&(bars->sl_phase_5).cv, &(bars->sl_phase_5).mutex);
#line 586
			if (Error != 0) {
#line 586
				break;
#line 586
			}
#line 586
		}
#line 586
		pthread_setcancelstate(Cancel, &Temp);
#line 586
	} else {
#line 586
		(bars->sl_phase_5).cycle = !(bars->sl_phase_5).cycle;
#line 586
		(bars->sl_phase_5).counter = 0;
#line 586
		Error = pthread_cond_broadcast(&(bars->sl_phase_5).cv);
#line 586
	}
#line 586
	pthread_mutex_unlock(&(bars->sl_phase_5).mutex);
#line 586
}
#else
   {
#line 588
	unsigned long	Error, Cycle;
#line 588
	long		Cancel, Temp;
#line 588

#line 588
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 588
	if (Error != 0) {
#line 588
		printf("Error while trying to get lock in barrier.\n");
#line 588
		exit(-1);
#line 588
	}
#line 588

#line 588
	Cycle = (bars->barrier).cycle;
#line 588
	if (++(bars->barrier).counter != (nprocs)) {
#line 588
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 588
		while (Cycle == (bars->barrier).cycle) {
#line 588
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 588
			if (Error != 0) {
#line 588
				break;
#line 588
			}
#line 588
		}
#line 588
		pthread_setcancelstate(Cancel, &Temp);
#line 588
	} else {
#line 588
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 588
		(bars->barrier).counter = 0;
#line 588
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 588
	}
#line 588
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 588
}
#endif
/*     *******************************************************

               s i x t h   p h a s e

       *******************************************************  */

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
       multi->rhs_multi[numlev-1][i][j] = wrk1->ga[i][j] * ressqr;
     }
   }
   if (istart == 0) {
     for(j=jstart;j<=jend;j++) {
       multi->q_multi[numlev-1][0][j] = wrk1->ga[0][j];
     }
   }
   if (iend == im-1) {
     for(j=jstart;j<=jend;j++) {
       multi->q_multi[numlev-1][im-1][j] = wrk1->ga[im-1][j];
     }
   }
   if (jstart == 0) {
     for(i=istart;i<=iend;i++) {
       multi->q_multi[numlev-1][i][0] = wrk1->ga[i][0];
     }
   }
   if (jend == jm-1) {
     for(i=istart;i<=iend;i++) {
       multi->q_multi[numlev-1][i][jm-1] = wrk1->ga[i][jm-1];
     }
   }

   fac = 1.0 / (4.0 - ressqr*eig2);
   for(i=ist;i<=ien;i++) {
     for(j=jst;j<=jen;j++) {
       multi->q_multi[numlev-1][i][j] = guess->oldga[i][j];
     }
   }

   if ((procid == MASTER) || (do_stats)) {
     {
#line 650
	struct timeval	FullTime;
#line 650

#line 650
	gettimeofday(&FullTime, NULL);
#line 650
	(multi_start) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 650
};
   }

   multig(procid);

   if ((procid == MASTER) || (do_stats)) {
     {
#line 656
	struct timeval	FullTime;
#line 656

#line 656
	gettimeofday(&FullTime, NULL);
#line 656
	(multi_end) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 656
};
     gp[procid].multi_time += (multi_end - multi_start);
   }

   if (procid == MASTER) {
     global->psiai=0.0;
   }

/*  copy the solution for use as initial guess in next time-step  */

   for(i=istart;i<=iend;i++) {
     for(j=jstart;j<=jend;j++) {
       wrk1->ga[i][j] = multi->q_multi[numlev-1][i][j];
       guess->oldga[i][j] = multi->q_multi[numlev-1][i][j];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 673
	unsigned long	Error, Cycle;
#line 673
	long		Cancel, Temp;
#line 673

#line 673
	Error = pthread_mutex_lock(&(bars->sl_phase_6).mutex);
#line 673
	if (Error != 0) {
#line 673
		printf("Error while trying to get lock in barrier.\n");
#line 673
		exit(-1);
#line 673
	}
#line 673

#line 673
	Cycle = (bars->sl_phase_6).cycle;
#line 673
	if (++(bars->sl_phase_6).counter != (nprocs)) {
#line 673
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 673
		while (Cycle == (bars->sl_phase_6).cycle) {
#line 673
			Error = pthread_cond_wait(&(bars->sl_phase_6).cv, &(bars->sl_phase_6).mutex);
#line 673
			if (Error != 0) {
#line 673
				break;
#line 673
			}
#line 673
		}
#line 673
		pthread_setcancelstate(Cancel, &Temp);
#line 673
	} else {
#line 673
		(bars->sl_phase_6).cycle = !(bars->sl_phase_6).cycle;
#line 673
		(bars->sl_phase_6).counter = 0;
#line 673
		Error = pthread_cond_broadcast(&(bars->sl_phase_6).cv);
#line 673
	}
#line 673
	pthread_mutex_unlock(&(bars->sl_phase_6).mutex);
#line 673
}
#else
   {
#line 675
	unsigned long	Error, Cycle;
#line 675
	long		Cancel, Temp;
#line 675

#line 675
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 675
	if (Error != 0) {
#line 675
		printf("Error while trying to get lock in barrier.\n");
#line 675
		exit(-1);
#line 675
	}
#line 675

#line 675
	Cycle = (bars->barrier).cycle;
#line 675
	if (++(bars->barrier).counter != (nprocs)) {
#line 675
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 675
		while (Cycle == (bars->barrier).cycle) {
#line 675
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 675
			if (Error != 0) {
#line 675
				break;
#line 675
			}
#line 675
		}
#line 675
		pthread_setcancelstate(Cancel, &Temp);
#line 675
	} else {
#line 675
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 675
		(bars->barrier).counter = 0;
#line 675
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 675
	}
#line 675
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 675
}
#endif
/*     *******************************************************

                s e v e n t h   p h a s e

       *******************************************************

   every process computes the running sum for its assigned portion
   in a private variable psiaipriv   */



   psiaipriv=0.0;
   if (procid == MASTER) {
     psiaipriv = psiaipriv + 0.25*(wrk1->ga[0][0]);
   }
   if (procid == xprocs - 1) {
     psiaipriv = psiaipriv + 0.25*(wrk1->ga[0][jm-1]);
   }
   if (procid == nprocs-xprocs) {
     psiaipriv=psiaipriv+0.25*(wrk1->ga[im-1][0]);
   }
   if (procid == nprocs-1) {
     psiaipriv=psiaipriv+0.25*(wrk1->ga[im-1][jm-1]);
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       psiaipriv = psiaipriv + 0.5*wrk1->ga[0][j];
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       psiaipriv = psiaipriv + 0.5*wrk1->ga[im-1][j];
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       psiaipriv = psiaipriv + 0.5*wrk1->ga[j][0];
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       psiaipriv = psiaipriv + 0.5*wrk1->ga[j][jm-1];
     }
   }
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
   for(i=firstrow;i<=lastrow;i++) {
       psiaipriv = psiaipriv + wrk1->ga[i][iindex];
     }
   }

/* after computing its private sum, every process adds that to the
   shared running sum psiai  */

   {pthread_mutex_lock(&(locks->psibilock));}
   global->psiai = global->psiai + psiaipriv;
   {pthread_mutex_unlock(&(locks->psibilock));}
#if defined(MULTIPLE_BARRIERS)
   {
#line 734
	unsigned long	Error, Cycle;
#line 734
	long		Cancel, Temp;
#line 734

#line 734
	Error = pthread_mutex_lock(&(bars->sl_phase_7).mutex);
#line 734
	if (Error != 0) {
#line 734
		printf("Error while trying to get lock in barrier.\n");
#line 734
		exit(-1);
#line 734
	}
#line 734

#line 734
	Cycle = (bars->sl_phase_7).cycle;
#line 734
	if (++(bars->sl_phase_7).counter != (nprocs)) {
#line 734
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 734
		while (Cycle == (bars->sl_phase_7).cycle) {
#line 734
			Error = pthread_cond_wait(&(bars->sl_phase_7).cv, &(bars->sl_phase_7).mutex);
#line 734
			if (Error != 0) {
#line 734
				break;
#line 734
			}
#line 734
		}
#line 734
		pthread_setcancelstate(Cancel, &Temp);
#line 734
	} else {
#line 734
		(bars->sl_phase_7).cycle = !(bars->sl_phase_7).cycle;
#line 734
		(bars->sl_phase_7).counter = 0;
#line 734
		Error = pthread_cond_broadcast(&(bars->sl_phase_7).cv);
#line 734
	}
#line 734
	pthread_mutex_unlock(&(bars->sl_phase_7).mutex);
#line 734
}
#else
   {
#line 736
	unsigned long	Error, Cycle;
#line 736
	long		Cancel, Temp;
#line 736

#line 736
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 736
	if (Error != 0) {
#line 736
		printf("Error while trying to get lock in barrier.\n");
#line 736
		exit(-1);
#line 736
	}
#line 736

#line 736
	Cycle = (bars->barrier).cycle;
#line 736
	if (++(bars->barrier).counter != (nprocs)) {
#line 736
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 736
		while (Cycle == (bars->barrier).cycle) {
#line 736
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 736
			if (Error != 0) {
#line 736
				break;
#line 736
			}
#line 736
		}
#line 736
		pthread_setcancelstate(Cancel, &Temp);
#line 736
	} else {
#line 736
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 736
		(bars->barrier).counter = 0;
#line 736
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 736
	}
#line 736
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 736
}
#endif
/*      *******************************************************

                e i g h t h   p h a s e

        *******************************************************

   augment ga(i,j) with [-psiai/psibi]*psib(i,j)

   %%%%%%%%%%%%%%% f4 should be private  */

   f4 = (-global->psiai)/(global->psibi);

   if (procid == MASTER) {
     wrk1->ga[0][0] = wrk1->ga[0][0]+f4*wrk1->psib[0][0];
   }
   if (procid == nprocs-xprocs) {
     wrk1->ga[im-1][0] = wrk1->ga[im-1][0]+f4*wrk1->psib[im-1][0];
   }
   if (procid == xprocs-1) {
     wrk1->ga[0][jm-1] = wrk1->ga[0][jm-1]+f4*wrk1->psib[0][jm-1];
   }
   if (procid == nprocs-1) {
     wrk1->ga[im-1][jm-1] = wrk1->ga[im-1][jm-1]+f4*wrk1->psib[im-1][jm-1];
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk1->ga[0][j] = wrk1->ga[0][j]+f4*wrk1->psib[0][j];
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk1->ga[im-1][j] = wrk1->ga[im-1][j]+f4*wrk1->psib[im-1][j];
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk1->ga[j][0] = wrk1->ga[j][0]+f4*wrk1->psib[j][0];
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk1->ga[j][jm-1] = wrk1->ga[j][jm-1]+f4*wrk1->psib[j][jm-1];
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       wrk1->ga[i][iindex] = wrk1->ga[i][iindex]+f4*wrk1->psib[i][iindex];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 788
	unsigned long	Error, Cycle;
#line 788
	long		Cancel, Temp;
#line 788

#line 788
	Error = pthread_mutex_lock(&(bars->sl_phase_8).mutex);
#line 788
	if (Error != 0) {
#line 788
		printf("Error while trying to get lock in barrier.\n");
#line 788
		exit(-1);
#line 788
	}
#line 788

#line 788
	Cycle = (bars->sl_phase_8).cycle;
#line 788
	if (++(bars->sl_phase_8).counter != (nprocs)) {
#line 788
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 788
		while (Cycle == (bars->sl_phase_8).cycle) {
#line 788
			Error = pthread_cond_wait(&(bars->sl_phase_8).cv, &(bars->sl_phase_8).mutex);
#line 788
			if (Error != 0) {
#line 788
				break;
#line 788
			}
#line 788
		}
#line 788
		pthread_setcancelstate(Cancel, &Temp);
#line 788
	} else {
#line 788
		(bars->sl_phase_8).cycle = !(bars->sl_phase_8).cycle;
#line 788
		(bars->sl_phase_8).counter = 0;
#line 788
		Error = pthread_cond_broadcast(&(bars->sl_phase_8).cv);
#line 788
	}
#line 788
	pthread_mutex_unlock(&(bars->sl_phase_8).mutex);
#line 788
}
#else
   {
#line 790
	unsigned long	Error, Cycle;
#line 790
	long		Cancel, Temp;
#line 790

#line 790
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 790
	if (Error != 0) {
#line 790
		printf("Error while trying to get lock in barrier.\n");
#line 790
		exit(-1);
#line 790
	}
#line 790

#line 790
	Cycle = (bars->barrier).cycle;
#line 790
	if (++(bars->barrier).counter != (nprocs)) {
#line 790
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 790
		while (Cycle == (bars->barrier).cycle) {
#line 790
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 790
			if (Error != 0) {
#line 790
				break;
#line 790
			}
#line 790
		}
#line 790
		pthread_setcancelstate(Cancel, &Temp);
#line 790
	} else {
#line 790
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 790
		(bars->barrier).counter = 0;
#line 790
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 790
	}
#line 790
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 790
}
#endif
   for(i=istart;i<=iend;i++) {
     for(j=jstart;j<=jend;j++) {
       multi->rhs_multi[numlev-1][i][j] = wrk1->gb[i][j] * ressqr;
     }
   }
   if (istart == 0) {
     for(j=jstart;j<=jend;j++) {
       multi->q_multi[numlev-1][0][j] = wrk1->gb[0][j];
     }
   }
   if (iend == im-1) {
     for(j=jstart;j<=jend;j++) {
       multi->q_multi[numlev-1][im-1][j] = wrk1->gb[im-1][j];
     }
   }
   if (jstart == 0) {
     for(i=istart;i<=iend;i++) {
       multi->q_multi[numlev-1][i][0] = wrk1->gb[i][0];
     }
   }
   if (jend == jm-1) {
     for(i=istart;i<=iend;i++) {
       multi->q_multi[numlev-1][i][jm-1] = wrk1->gb[i][jm-1];
     }
   }

   fac = 1.0 / (4.0 - ressqr*eig2);
   for(i=ist;i<=ien;i++) {
     for(j=jst;j<=jen;j++) {
       multi->q_multi[numlev-1][i][j] = guess->oldgb[i][j];
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
	(multi_start) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 826
};
   }

   multig(procid);

   if ((procid == MASTER) || (do_stats)) {
     {
#line 832
	struct timeval	FullTime;
#line 832

#line 832
	gettimeofday(&FullTime, NULL);
#line 832
	(multi_end) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 832
};
     gp[procid].multi_time += (multi_end - multi_start);
   }

   for(i=istart;i<=iend;i++) {
     for(j=jstart;j<=jend;j++) {
       wrk1->gb[i][j] = multi->q_multi[numlev-1][i][j];
       guess->oldgb[i][j] = multi->q_multi[numlev-1][i][j];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 843
	unsigned long	Error, Cycle;
#line 843
	long		Cancel, Temp;
#line 843

#line 843
	Error = pthread_mutex_lock(&(bars->sl_phase_8).mutex);
#line 843
	if (Error != 0) {
#line 843
		printf("Error while trying to get lock in barrier.\n");
#line 843
		exit(-1);
#line 843
	}
#line 843

#line 843
	Cycle = (bars->sl_phase_8).cycle;
#line 843
	if (++(bars->sl_phase_8).counter != (nprocs)) {
#line 843
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 843
		while (Cycle == (bars->sl_phase_8).cycle) {
#line 843
			Error = pthread_cond_wait(&(bars->sl_phase_8).cv, &(bars->sl_phase_8).mutex);
#line 843
			if (Error != 0) {
#line 843
				break;
#line 843
			}
#line 843
		}
#line 843
		pthread_setcancelstate(Cancel, &Temp);
#line 843
	} else {
#line 843
		(bars->sl_phase_8).cycle = !(bars->sl_phase_8).cycle;
#line 843
		(bars->sl_phase_8).counter = 0;
#line 843
		Error = pthread_cond_broadcast(&(bars->sl_phase_8).cv);
#line 843
	}
#line 843
	pthread_mutex_unlock(&(bars->sl_phase_8).mutex);
#line 843
}
#else
   {
#line 845
	unsigned long	Error, Cycle;
#line 845
	long		Cancel, Temp;
#line 845

#line 845
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 845
	if (Error != 0) {
#line 845
		printf("Error while trying to get lock in barrier.\n");
#line 845
		exit(-1);
#line 845
	}
#line 845

#line 845
	Cycle = (bars->barrier).cycle;
#line 845
	if (++(bars->barrier).counter != (nprocs)) {
#line 845
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 845
		while (Cycle == (bars->barrier).cycle) {
#line 845
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 845
			if (Error != 0) {
#line 845
				break;
#line 845
			}
#line 845
		}
#line 845
		pthread_setcancelstate(Cancel, &Temp);
#line 845
	} else {
#line 845
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 845
		(bars->barrier).counter = 0;
#line 845
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 845
	}
#line 845
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 845
}
#endif
/*      *******************************************************

                n i n t h   p h a s e

        *******************************************************

   put appropriate linear combinations of ga and gb in work2 and work3;
   note that here (as in most cases) the constant multipliers are made
   private variables; the specific order in which things are done is
   chosen in order to hopefully reuse things brought into the cache

   note that here again we choose to have all processes share the work
   on both matrices despite the fact that the work done per element
   is the same, because the operand matrices are the same in both cases */

   if (procid == MASTER) {
     wrk3->work2[0][0] = wrk1->gb[0][0]-hh1*wrk1->ga[0][0];
     wrk2->work3[0][0] = wrk1->gb[0][0]+hh3*wrk1->ga[0][0];
   }
   if (procid == nprocs-xprocs) {
     wrk3->work2[im-1][0] = wrk1->gb[im-1][0]-hh1*wrk1->ga[im-1][0];
     wrk2->work3[im-1][0] = wrk1->gb[im-1][0]+hh3*wrk1->ga[im-1][0];
   }
   if (procid == xprocs-1) {
     wrk3->work2[0][jm-1] = wrk1->gb[0][jm-1]-hh1*wrk1->ga[0][jm-1];
     wrk2->work3[0][jm-1] = wrk1->gb[0][jm-1]+hh3*wrk1->ga[0][jm-1];
   }
   if (procid == nprocs-1) {
     wrk3->work2[im-1][jm-1] = wrk1->gb[im-1][jm-1]-hh1*wrk1->ga[im-1][jm-1];
     wrk2->work3[im-1][jm-1] = wrk1->gb[im-1][jm-1]+hh3*wrk1->ga[im-1][jm-1];
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk2->work3[0][j] = wrk1->gb[0][j]+hh3*wrk1->ga[0][j];
       wrk3->work2[0][j] = wrk1->gb[0][j]-hh1*wrk1->ga[0][j];
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       wrk2->work3[im-1][j] = wrk1->gb[im-1][j]+hh3*wrk1->ga[im-1][j];
       wrk3->work2[im-1][j] = wrk1->gb[im-1][j]-hh1*wrk1->ga[im-1][j];
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk2->work3[j][0] = wrk1->gb[j][0]+hh3*wrk1->ga[j][0];
       wrk3->work2[j][0] = wrk1->gb[j][0]-hh1*wrk1->ga[j][0];
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       wrk2->work3[j][jm-1] = wrk1->gb[j][jm-1]+hh3*wrk1->ga[j][jm-1];
       wrk3->work2[j][jm-1] = wrk1->gb[j][jm-1]-hh1*wrk1->ga[j][jm-1];
     }
   }

   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       wrk2->work3[i][iindex] = wrk1->gb[i][iindex]+hh3*wrk1->ga[i][iindex];
       wrk3->work2[i][iindex] = wrk1->gb[i][iindex]-hh1*wrk1->ga[i][iindex];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 910
	unsigned long	Error, Cycle;
#line 910
	long		Cancel, Temp;
#line 910

#line 910
	Error = pthread_mutex_lock(&(bars->sl_phase_9).mutex);
#line 910
	if (Error != 0) {
#line 910
		printf("Error while trying to get lock in barrier.\n");
#line 910
		exit(-1);
#line 910
	}
#line 910

#line 910
	Cycle = (bars->sl_phase_9).cycle;
#line 910
	if (++(bars->sl_phase_9).counter != (nprocs)) {
#line 910
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 910
		while (Cycle == (bars->sl_phase_9).cycle) {
#line 910
			Error = pthread_cond_wait(&(bars->sl_phase_9).cv, &(bars->sl_phase_9).mutex);
#line 910
			if (Error != 0) {
#line 910
				break;
#line 910
			}
#line 910
		}
#line 910
		pthread_setcancelstate(Cancel, &Temp);
#line 910
	} else {
#line 910
		(bars->sl_phase_9).cycle = !(bars->sl_phase_9).cycle;
#line 910
		(bars->sl_phase_9).counter = 0;
#line 910
		Error = pthread_cond_broadcast(&(bars->sl_phase_9).cv);
#line 910
	}
#line 910
	pthread_mutex_unlock(&(bars->sl_phase_9).mutex);
#line 910
}
#else
   {
#line 912
	unsigned long	Error, Cycle;
#line 912
	long		Cancel, Temp;
#line 912

#line 912
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 912
	if (Error != 0) {
#line 912
		printf("Error while trying to get lock in barrier.\n");
#line 912
		exit(-1);
#line 912
	}
#line 912

#line 912
	Cycle = (bars->barrier).cycle;
#line 912
	if (++(bars->barrier).counter != (nprocs)) {
#line 912
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 912
		while (Cycle == (bars->barrier).cycle) {
#line 912
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 912
			if (Error != 0) {
#line 912
				break;
#line 912
			}
#line 912
		}
#line 912
		pthread_setcancelstate(Cancel, &Temp);
#line 912
	} else {
#line 912
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 912
		(bars->barrier).counter = 0;
#line 912
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 912
	}
#line 912
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 912
}
#endif
/*      *******************************************************

                t e n t h    p h a s e

        *******************************************************/


   timst = 2*dtau;

/* update the psi{1,3} matrices by adding 2*dtau*work3 to each */

   if (procid == MASTER) {
     fields->psi[0][0][0] = fields->psi[0][0][0] + timst*wrk2->work3[0][0];
   }
   if (procid == nprocs-xprocs) {
     fields->psi[0][im-1][0] = fields->psi[0][im-1][0] + timst*wrk2->work3[im-1][0];
   }
   if (procid == xprocs-1) {
     fields->psi[0][0][jm-1] = fields->psi[0][0][jm-1] + timst*wrk2->work3[0][jm-1];
   }
   if (procid == nprocs-1) {
     fields->psi[0][im-1][jm-1] = fields->psi[0][im-1][jm-1] + timst*wrk2->work3[im-1][jm-1];
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       fields->psi[0][0][j] = fields->psi[0][0][j] + timst*wrk2->work3[0][j];
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       fields->psi[0][im-1][j] = fields->psi[0][im-1][j] + timst*wrk2->work3[im-1][j];
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       fields->psi[0][j][0] = fields->psi[0][j][0] + timst*wrk2->work3[j][0];
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       fields->psi[0][j][jm-1] = fields->psi[0][j][jm-1] + timst*wrk2->work3[j][jm-1];
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
         fields->psi[0][i][iindex] = fields->psi[0][i][iindex] + timst*wrk2->work3[i][iindex];
     }
   }

   if (procid == MASTER) {
     fields->psi[1][0][0] = fields->psi[1][0][0] + timst*wrk3->work2[0][0];
   }
   if (procid == nprocs-xprocs) {
     fields->psi[1][im-1][0] = fields->psi[1][im-1][0] + timst*wrk3->work2[im-1][0];
   }
   if (procid == xprocs-1) {
     fields->psi[1][0][jm-1] = fields->psi[1][0][jm-1] + timst*wrk3->work2[0][jm-1];
   }
   if (procid == nprocs-1) {
     fields->psi[1][im-1][jm-1] = fields->psi[1][im-1][jm-1] + timst*wrk3->work2[im-1][jm-1];
   }
   if (firstrow == 1) {
     for(j=firstcol;j<=lastcol;j++) {
       fields->psi[1][0][j] = fields->psi[1][0][j] + timst*wrk3->work2[0][j];
     }
   }
   if ((firstrow+numrows) == im-1) {
     for(j=firstcol;j<=lastcol;j++) {
       fields->psi[1][im-1][j] = fields->psi[1][im-1][j] + timst*wrk3->work2[im-1][j];
     }
   }
   if (firstcol == 1) {
     for(j=firstrow;j<=lastrow;j++) {
       fields->psi[1][j][0] = fields->psi[1][j][0] + timst*wrk3->work2[j][0];
     }
   }
   if ((firstcol+numcols) == jm-1) {
     for(j=firstrow;j<=lastrow;j++) {
       fields->psi[1][j][jm-1] = fields->psi[1][j][jm-1] + timst*wrk3->work2[j][jm-1];
     }
   }

   for(i=firstrow;i<=lastrow;i++) {
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
         fields->psi[1][i][iindex] = fields->psi[1][i][iindex] + timst*wrk3->work2[i][iindex];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 1002
	unsigned long	Error, Cycle;
#line 1002
	long		Cancel, Temp;
#line 1002

#line 1002
	Error = pthread_mutex_lock(&(bars->sl_phase_10).mutex);
#line 1002
	if (Error != 0) {
#line 1002
		printf("Error while trying to get lock in barrier.\n");
#line 1002
		exit(-1);
#line 1002
	}
#line 1002

#line 1002
	Cycle = (bars->sl_phase_10).cycle;
#line 1002
	if (++(bars->sl_phase_10).counter != (nprocs)) {
#line 1002
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 1002
		while (Cycle == (bars->sl_phase_10).cycle) {
#line 1002
			Error = pthread_cond_wait(&(bars->sl_phase_10).cv, &(bars->sl_phase_10).mutex);
#line 1002
			if (Error != 0) {
#line 1002
				break;
#line 1002
			}
#line 1002
		}
#line 1002
		pthread_setcancelstate(Cancel, &Temp);
#line 1002
	} else {
#line 1002
		(bars->sl_phase_10).cycle = !(bars->sl_phase_10).cycle;
#line 1002
		(bars->sl_phase_10).counter = 0;
#line 1002
		Error = pthread_cond_broadcast(&(bars->sl_phase_10).cv);
#line 1002
	}
#line 1002
	pthread_mutex_unlock(&(bars->sl_phase_10).mutex);
#line 1002
}
#else
   {
#line 1004
	unsigned long	Error, Cycle;
#line 1004
	long		Cancel, Temp;
#line 1004

#line 1004
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 1004
	if (Error != 0) {
#line 1004
		printf("Error while trying to get lock in barrier.\n");
#line 1004
		exit(-1);
#line 1004
	}
#line 1004

#line 1004
	Cycle = (bars->barrier).cycle;
#line 1004
	if (++(bars->barrier).counter != (nprocs)) {
#line 1004
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 1004
		while (Cycle == (bars->barrier).cycle) {
#line 1004
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 1004
			if (Error != 0) {
#line 1004
				break;
#line 1004
			}
#line 1004
		}
#line 1004
		pthread_setcancelstate(Cancel, &Temp);
#line 1004
	} else {
#line 1004
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 1004
		(bars->barrier).counter = 0;
#line 1004
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 1004
	}
#line 1004
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 1004
}
#endif
}
