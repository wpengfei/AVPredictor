#line 228 "../null_macros/c.m4.null"

#line 1 "mdmain.C"
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


#line 17
#include <pthread.h>
#line 17
#include <sys/time.h>
#line 17
#include <unistd.h>
#line 17
#include <stdlib.h>
#line 17
extern pthread_t PThreadTable[];
#line 17


#include <stdio.h>

#include "parameters.h"
#include "mdvar.h"
#include "water.h"
#include "wwpot.h"
#include "cnst.h"
#include "mddata.h"
#include "fileio.h"
#include "split.h"
#include "global.h"

/************************************************************************/

double MDMAIN(long NSTEP, long NPRINT, long NSAVE, long NORD1, long ProcID)
{
    double TVIR = 0.0;
    double TTMV = 0.0;
    double TKIN = 0.0;
    double XTT;
    long i,j,k;
    double POTA,POTR,POTRF;
    double XVIR,AVGT,TEN;
    struct list_of_boxes *new_box, *curr_box;

    for (i=start_end[ProcID]->box[XDIR][FIRST]; i<=start_end[ProcID]->box[XDIR][LAST]; i++) {
        for (j=start_end[ProcID]->box[YDIR][FIRST]; j<=start_end[ProcID]->box[YDIR][LAST]; j++) {
            for (k=start_end[ProcID]->box[ZDIR][FIRST]; k<=start_end[ProcID]->box[ZDIR][LAST]; k++) {
                new_box = (box_list *) malloc(sizeof(box_list));;
                new_box->coord[XDIR] = i;
                new_box->coord[YDIR] = j;
                new_box->coord[ZDIR] = k;
                new_box->next_box = NULL;
                curr_box = my_boxes[ProcID];
                if (curr_box == NULL)
                    my_boxes[ProcID] = new_box;
                else {
                    while (curr_box->next_box != NULL)
                        curr_box = curr_box->next_box;
                    curr_box->next_box = new_box;
                } /* else */
            }
        }
    }

    /* calculate initial value for acceleration */

    INTRAF(&gl->VIR,ProcID);

    {
#line 68
	unsigned long	Error, Cycle;
#line 68
	long		Cancel, Temp;
#line 68

#line 68
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 68
	if (Error != 0) {
#line 68
		printf("Error while trying to get lock in barrier.\n");
#line 68
		exit(-1);
#line 68
	}
#line 68

#line 68
	Cycle = (gl->start).cycle;
#line 68
	if (++(gl->start).counter != (NumProcs)) {
#line 68
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 68
		while (Cycle == (gl->start).cycle) {
#line 68
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 68
			if (Error != 0) {
#line 68
				break;
#line 68
			}
#line 68
		}
#line 68
		pthread_setcancelstate(Cancel, &Temp);
#line 68
	} else {
#line 68
		(gl->start).cycle = !(gl->start).cycle;
#line 68
		(gl->start).counter = 0;
#line 68
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 68
	}
#line 68
	pthread_mutex_unlock(&(gl->start).mutex);
#line 68
};

    INTERF(ACC,&gl->VIR,ProcID);

    {
#line 72
	unsigned long	Error, Cycle;
#line 72
	long		Cancel, Temp;
#line 72

#line 72
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 72
	if (Error != 0) {
#line 72
		printf("Error while trying to get lock in barrier.\n");
#line 72
		exit(-1);
#line 72
	}
#line 72

#line 72
	Cycle = (gl->start).cycle;
#line 72
	if (++(gl->start).counter != (NumProcs)) {
#line 72
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 72
		while (Cycle == (gl->start).cycle) {
#line 72
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 72
			if (Error != 0) {
#line 72
				break;
#line 72
			}
#line 72
		}
#line 72
		pthread_setcancelstate(Cancel, &Temp);
#line 72
	} else {
#line 72
		(gl->start).cycle = !(gl->start).cycle;
#line 72
		(gl->start).counter = 0;
#line 72
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 72
	}
#line 72
	pthread_mutex_unlock(&(gl->start).mutex);
#line 72
};

    /* MOLECULAR DYNAMICS LOOP */

    for (i=1;i <= NSTEP; i++) {
        TTMV=TTMV+1.00;

        /* POSSIBLE ENHANCEMENT:  Here's where one start measurements to avoid
           cold-start effects.  Recommended to do this at the beginning of the
           second timestep; i.e. if (i == 2).
           */

        /* initialize various shared sums */
        if (ProcID == 0) {
            long dir;
            if (i >= 2) {
                {
#line 88
	struct timeval	FullTime;
#line 88

#line 88
	gettimeofday(&FullTime, NULL);
#line 88
	(gl->trackstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 88
};
            }
            gl->VIR = 0.0;
            gl->POTA = 0.0;
            gl->POTR = 0.0;
            gl->POTRF = 0.0;
            for (dir = XDIR; dir <= ZDIR; dir++)
                gl->SUM[dir] = 0.0;
        }

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 99
	struct timeval	FullTime;
#line 99

#line 99
	gettimeofday(&FullTime, NULL);
#line 99
	(gl->intrastart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 99
};
        }

        {
#line 102
	unsigned long	Error, Cycle;
#line 102
	long		Cancel, Temp;
#line 102

#line 102
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 102
	if (Error != 0) {
#line 102
		printf("Error while trying to get lock in barrier.\n");
#line 102
		exit(-1);
#line 102
	}
#line 102

#line 102
	Cycle = (gl->start).cycle;
#line 102
	if (++(gl->start).counter != (NumProcs)) {
#line 102
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 102
		while (Cycle == (gl->start).cycle) {
#line 102
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 102
			if (Error != 0) {
#line 102
				break;
#line 102
			}
#line 102
		}
#line 102
		pthread_setcancelstate(Cancel, &Temp);
#line 102
	} else {
#line 102
		(gl->start).cycle = !(gl->start).cycle;
#line 102
		(gl->start).counter = 0;
#line 102
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 102
	}
#line 102
	pthread_mutex_unlock(&(gl->start).mutex);
#line 102
};

        PREDIC(TLC,NORD1,ProcID);
        INTRAF(&gl->VIR,ProcID);

        {
#line 107
	unsigned long	Error, Cycle;
#line 107
	long		Cancel, Temp;
#line 107

#line 107
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 107
	if (Error != 0) {
#line 107
		printf("Error while trying to get lock in barrier.\n");
#line 107
		exit(-1);
#line 107
	}
#line 107

#line 107
	Cycle = (gl->start).cycle;
#line 107
	if (++(gl->start).counter != (NumProcs)) {
#line 107
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 107
		while (Cycle == (gl->start).cycle) {
#line 107
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 107
			if (Error != 0) {
#line 107
				break;
#line 107
			}
#line 107
		}
#line 107
		pthread_setcancelstate(Cancel, &Temp);
#line 107
	} else {
#line 107
		(gl->start).cycle = !(gl->start).cycle;
#line 107
		(gl->start).counter = 0;
#line 107
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 107
	}
#line 107
	pthread_mutex_unlock(&(gl->start).mutex);
#line 107
};

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 110
	struct timeval	FullTime;
#line 110

#line 110
	gettimeofday(&FullTime, NULL);
#line 110
	(gl->intraend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 110
};
            gl->intratime += gl->intraend - gl->intrastart;
        }

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 115
	struct timeval	FullTime;
#line 115

#line 115
	gettimeofday(&FullTime, NULL);
#line 115
	(gl->interstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 115
};
        }

        INTERF(FORCES,&gl->VIR,ProcID);

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 121
	struct timeval	FullTime;
#line 121

#line 121
	gettimeofday(&FullTime, NULL);
#line 121
	(gl->interend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 121
};
            gl->intertime += gl->interend - gl->interstart;
        }

        CORREC(PCC,NORD1,ProcID);

        BNDRY(ProcID);

        KINETI(gl->SUM,HMAS,OMAS,ProcID);

        {
#line 131
	unsigned long	Error, Cycle;
#line 131
	long		Cancel, Temp;
#line 131

#line 131
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 131
	if (Error != 0) {
#line 131
		printf("Error while trying to get lock in barrier.\n");
#line 131
		exit(-1);
#line 131
	}
#line 131

#line 131
	Cycle = (gl->start).cycle;
#line 131
	if (++(gl->start).counter != (NumProcs)) {
#line 131
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 131
		while (Cycle == (gl->start).cycle) {
#line 131
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 131
			if (Error != 0) {
#line 131
				break;
#line 131
			}
#line 131
		}
#line 131
		pthread_setcancelstate(Cancel, &Temp);
#line 131
	} else {
#line 131
		(gl->start).cycle = !(gl->start).cycle;
#line 131
		(gl->start).counter = 0;
#line 131
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 131
	}
#line 131
	pthread_mutex_unlock(&(gl->start).mutex);
#line 131
};

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 134
	struct timeval	FullTime;
#line 134

#line 134
	gettimeofday(&FullTime, NULL);
#line 134
	(gl->intraend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 134
};
            gl->intratime += gl->intraend - gl->interend;
        }

        TKIN=TKIN+gl->SUM[0]+gl->SUM[1]+gl->SUM[2];
        TVIR=TVIR-gl->VIR;

        /* CHECK if  PRINTING AND/OR SAVING IS TO BE DONE */

        if ( ((i % NPRINT) == 0) || ((NSAVE > 0) && ((i % NSAVE) == 0))) {

            /* if so, call poteng to compute potential energy.  Note
               that we are attributing all the time in poteng to intermolecular
               computation although some of it is intramolecular (see poteng.C) */

            if ((ProcID == 0) && (i >= 2)) {
                {
#line 150
	struct timeval	FullTime;
#line 150

#line 150
	gettimeofday(&FullTime, NULL);
#line 150
	(gl->interstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 150
};
            }

            POTENG(&gl->POTA,&gl->POTR,&gl->POTRF,ProcID);

            {
#line 155
	unsigned long	Error, Cycle;
#line 155
	long		Cancel, Temp;
#line 155

#line 155
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 155
	if (Error != 0) {
#line 155
		printf("Error while trying to get lock in barrier.\n");
#line 155
		exit(-1);
#line 155
	}
#line 155

#line 155
	Cycle = (gl->start).cycle;
#line 155
	if (++(gl->start).counter != (NumProcs)) {
#line 155
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 155
		while (Cycle == (gl->start).cycle) {
#line 155
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 155
			if (Error != 0) {
#line 155
				break;
#line 155
			}
#line 155
		}
#line 155
		pthread_setcancelstate(Cancel, &Temp);
#line 155
	} else {
#line 155
		(gl->start).cycle = !(gl->start).cycle;
#line 155
		(gl->start).counter = 0;
#line 155
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 155
	}
#line 155
	pthread_mutex_unlock(&(gl->start).mutex);
#line 155
};

            if ((ProcID == 0) && (i >= 2)) {
                {
#line 158
	struct timeval	FullTime;
#line 158

#line 158
	gettimeofday(&FullTime, NULL);
#line 158
	(gl->interend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 158
};
                gl->intertime += gl->interend - gl->interstart;
            }

            POTA=gl->POTA*FPOT;
            POTR=gl->POTR*FPOT;
            POTRF=gl->POTRF*FPOT;
            XVIR=TVIR*FPOT*0.50/TTMV;
            AVGT=TKIN*FKIN*TEMP*2.00/(3.00*TTMV);
            TEN=(gl->SUM[0]+gl->SUM[1]+gl->SUM[2])*FKIN;
            XTT=POTA+POTR+POTRF+TEN;

            /* if it is time to print output as well ... */
            if ((i % NPRINT) == 0 && ProcID == 0) {
                {pthread_mutex_lock(&(gl->IOLock));};
                fprintf(six,"     %5ld %14.5lf %12.5lf %12.5lf %12.5lf \n"
                        ,i,TEN,POTA,POTR,POTRF);
                fprintf(six," %16.3lf %16.5lf %16.5lf\n",XTT,AVGT,XVIR);
                fflush(six);
                {pthread_mutex_unlock(&(gl->IOLock));};
            }

        }

        {
#line 182
	unsigned long	Error, Cycle;
#line 182
	long		Cancel, Temp;
#line 182

#line 182
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 182
	if (Error != 0) {
#line 182
		printf("Error while trying to get lock in barrier.\n");
#line 182
		exit(-1);
#line 182
	}
#line 182

#line 182
	Cycle = (gl->start).cycle;
#line 182
	if (++(gl->start).counter != (NumProcs)) {
#line 182
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 182
		while (Cycle == (gl->start).cycle) {
#line 182
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 182
			if (Error != 0) {
#line 182
				break;
#line 182
			}
#line 182
		}
#line 182
		pthread_setcancelstate(Cancel, &Temp);
#line 182
	} else {
#line 182
		(gl->start).cycle = !(gl->start).cycle;
#line 182
		(gl->start).counter = 0;
#line 182
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 182
	}
#line 182
	pthread_mutex_unlock(&(gl->start).mutex);
#line 182
};

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 185
	struct timeval	FullTime;
#line 185

#line 185
	gettimeofday(&FullTime, NULL);
#line 185
	(gl->trackend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 185
};
            gl->tracktime += gl->trackend - gl->trackstart;
        }

    } /* for i */

    return(XTT);

} /* mdmain.c */
