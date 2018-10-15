#line 228 "../null_macros/c.m4.null"

#line 1 "bndry.C"
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
#include "mdvar.h"
#include "parameters.h"
#include "mddata.h"
#include "split.h"
#include "global.h"

void BNDRY(long ProcID)     /* this routine puts the molecules back inside the box if they are out */
{
    long i, j, k, dir;
    long X_INDEX, Y_INDEX, Z_INDEX;
    struct link *curr_ptr, *last_ptr, *next_ptr, *temp_ptr;
    struct list_of_boxes *curr_box;
    double *extra_p;

    /* for each box */
    curr_box = my_boxes[ProcID];
    while (curr_box) {
        i = curr_box->coord[XDIR];  /* X coordinate of box */
        j = curr_box->coord[YDIR];  /* Y coordinate of box */
        k = curr_box->coord[ZDIR];  /* Z coordinate of box */

        last_ptr = NULL;
        curr_ptr = BOX[i][j][k].list;

        /* Go through molecules in current box */

        while (curr_ptr) {
            next_ptr = curr_ptr->next_mol;

            /* for each direction */

            for ( dir = XDIR; dir <= ZDIR; dir++ ) {
                extra_p = curr_ptr->mol.F[DISP][dir];

                /* if the oxygen atom is out of the box */
                if (extra_p[O] > BOXL ) {

                    /* move all three atoms back in the box */
                    extra_p[H1] -= BOXL;
                    extra_p[O]  -= BOXL;
                    extra_p[H2] -= BOXL;
                }
                else if (extra_p[O] < 0.00) {
                    extra_p[H1] += BOXL;
                    extra_p[O]  += BOXL;
                    extra_p[H2] += BOXL;
                }
            } /* for dir */

            /* If O atom moves out of current box, put it in correct box */
            X_INDEX = (long) (curr_ptr->mol.F[DISP][XDIR][O] / BOX_LENGTH);
            Y_INDEX = (long) (curr_ptr->mol.F[DISP][YDIR][O] / BOX_LENGTH);
            Z_INDEX = (long) (curr_ptr->mol.F[DISP][ZDIR][O] / BOX_LENGTH);

            if ((X_INDEX != i) ||
                (Y_INDEX != j) ||
                (Z_INDEX != k)) {

                /* Remove link from BOX[i][j][k] */

                {pthread_mutex_lock(&(BOX[i][j][k].boxlock));};
                if (last_ptr != NULL)
                    last_ptr->next_mol = curr_ptr->next_mol;
                else
                    BOX[i][j][k].list = curr_ptr->next_mol;
                {pthread_mutex_unlock(&(BOX[i][j][k].boxlock));};

                /* Add link to BOX[X_INDEX][Y_INDEX][Z_INDEX] */

                {pthread_mutex_lock(&(BOX[X_INDEX][Y_INDEX][Z_INDEX].boxlock));};
                temp_ptr = BOX[X_INDEX][Y_INDEX][Z_INDEX].list;
                BOX[X_INDEX][Y_INDEX][Z_INDEX].list = curr_ptr;
                curr_ptr->next_mol = temp_ptr;
                {pthread_mutex_unlock(&(BOX[X_INDEX][Y_INDEX][Z_INDEX].boxlock));};

            }
            else last_ptr = curr_ptr;
            curr_ptr = next_ptr;   /* Go to next molecule in current box */
        } /* while curr_ptr */
        curr_box = curr_box->next_box;
    } /* for curr_box */

} /* end of subroutine BNDRY */
