#line 228 "../null_macros/c.m4.null"

#line 1 "lu.C"
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

/*************************************************************************/
/*                                                                       */
/*  Parallel dense blocked LU factorization (no pivoting)                */
/*                                                                       */
/*  This version contains two dimensional arrays in which the first      */
/*  dimension is the block to be operated on, and the second contains    */
/*  all data points in that block.  In this manner, all data points in   */
/*  a block (which are operated on by the same processor) are allocated  */
/*  contiguously and locally, and false sharing is eliminated.           */
/*                                                                       */
/*  Command line options:                                                */
/*                                                                       */
/*  -nN : Decompose NxN matrix.                                          */
/*  -pP : P = number of processors.                                      */
/*  -bB : Use a block size of B. BxB elements should fit in cache for    */
/*        good performance. Small block sizes (B=8, B=16) work well.     */
/*  -s  : Print individual processor timing statistics.                  */
/*  -t  : Test output.                                                   */
/*  -o  : Print out matrix values.                                       */
/*  -h  : Print out command line options.                                */
/*                                                                       */
/*  Note: This version works under both the FORK and SPROC models        */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#line 45
#include <pthread.h>
#line 45
#include <sys/time.h>
#line 45
#include <unistd.h>
#line 45
#include <stdlib.h>
#line 45
#define MAX_THREADS 32
#line 45
pthread_t PThreadTable[MAX_THREADS];
#line 45


#define MAXRAND                         32767.0
#define DEFAULT_N                         512
#define DEFAULT_P                           1
#define DEFAULT_B                          16
#define min(a,b) ((a) < (b) ? (a) : (b))
//#define PAGE_SIZE                       4096
#define PAGE_SIZE			1024

struct GlobalMemory {
  double *t_in_fac;   
  double *t_in_solve;
  double *t_in_mod; 
  double *t_in_bar;
  double *completion;
  unsigned long starttime; 
  unsigned long rf; 
  unsigned long rs; 
  unsigned long done;
  long id;
  
#line 66
struct {
#line 66
	pthread_mutex_t	mutex;
#line 66
	pthread_cond_t	cv;
#line 66
	unsigned long	counter;
#line 66
	unsigned long	cycle;
#line 66
} (start);
#line 66

  pthread_mutex_t (idlock);
} *Global;

struct LocalCopies {
  double t_in_fac;
  double t_in_solve;
  double t_in_mod;
  double t_in_bar;
};

long n = DEFAULT_N;          /* The size of the matrix */
long P = DEFAULT_P;          /* Number of processors */
long block_size = DEFAULT_B; /* Block dimension */
long nblocks;                /* Number of blocks in each dimension */
long num_rows;               /* Number of processors per row of processor grid */
long num_cols;               /* Number of processors per col of processor grid */
double **a;                  /* a = lu; l and u both placed back in a */
double *rhs;
long *proc_bytes;            /* Bytes to malloc per processor to hold blocks of A*/
double **last_malloc;        /* Starting point of last block of A */

long test_result = 0;        /* Test result of factorization? */
long doprint = 0;            /* Print out matrix values? */
long dostats = 0;            /* Print out individual processor statistics? */

void SlaveStart(void);
void OneSolve(long n, long block_size, long MyNum, long dostats);
void lu0(double *a, long n, long stride);
void bdiv(double *a, double *diag, long stride_a, long stride_diag, long dimi, long dimk);
void bmodd(double *a, double *c, long dimi, long dimj, long stride_a, long stride_c);
void bmod(double *a, double *b, double *c, long dimi, long dimj, long dimk, long stridea, long strideb, long stridec);
void daxpy(double *a, double *b, long n, double alpha);
long BlockOwner(long I, long J);
long BlockOwnerColumn(long I, long J);
long BlockOwnerRow(long I, long J);
void lu(long n, long bs, long MyNum, struct LocalCopies *lc, long dostats);
void InitA(double *rhs);
double TouchA(long bs, long MyNum);
void PrintA(void);
void CheckResult(long n, double **a, double *rhs);
void printerr(char *s);

int main(int argc, char *argv[])
{
  long i, j;
  long ch;
  extern char *optarg;
  double mint, maxt, avgt;
  double min_fac, min_solve, min_mod, min_bar;
  double max_fac, max_solve, max_mod, max_bar;
  double avg_fac, avg_solve, avg_mod, avg_bar;
  long proc_num;
  long edge;
  long size;
  unsigned long start;

  {
#line 123
	struct timeval	FullTime;
#line 123

#line 123
	gettimeofday(&FullTime, NULL);
#line 123
	(start) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 123
}

  while ((ch = getopt(argc, argv, "n:p:b:cstoh")) != -1) {
    switch(ch) {
    case 'n': n = atoi(optarg); break;
    case 'p': P = atoi(optarg); break;
    case 'b': block_size = atoi(optarg); break;
    case 's': dostats = 1; break;
    case 't': test_result = !test_result; break;
    case 'o': doprint = !doprint; break;
    case 'h': printf("Usage: LU <options>\n\n");
	      printf("options:\n");
              printf("  -nN : Decompose NxN matrix.\n");
              printf("  -pP : P = number of processors.\n");
              printf("  -bB : Use a block size of B. BxB elements should fit in cache for \n");
              printf("        good performance. Small block sizes (B=8, B=16) work well.\n");
              printf("  -c  : Copy non-locally allocated blocks to local memory before use.\n");
              printf("  -s  : Print individual processor timing statistics.\n");
              printf("  -t  : Test output.\n");
              printf("  -o  : Print out matrix values.\n");
              printf("  -h  : Print out command line options.\n\n");
              printf("Default: LU -n%1d -p%1d -b%1d\n",
		     DEFAULT_N,DEFAULT_P,DEFAULT_B);
              exit(0);
              break;
    }
  }

  {;}

  printf("\n");
  printf("Blocked Dense LU Factorization\n");
  printf("     %ld by %ld Matrix\n",n,n);
  printf("     %ld Processors\n",P);
  printf("     %ld by %ld Element Blocks\n",block_size,block_size);
  printf("\n");
  printf("\n");

  num_rows = (long) sqrt((double) P);
  for (;;) {
    num_cols = P/num_rows;
    if (num_rows*num_cols == P)
      break;
    num_rows--;
  }
  nblocks = n/block_size;
  if (block_size * nblocks != n) {
    nblocks++;
  }

  edge = n%block_size;
  if (edge == 0) {
    edge = block_size;
  }

  proc_bytes = (long *) malloc(P*sizeof(long));
  if (proc_bytes == NULL) {
	  fprintf(stderr,"Could not malloc memory for proc_bytes.\n");
	  exit(-1);
  }
  last_malloc = (double **) malloc(P*sizeof(double *));;
  if (last_malloc == NULL) {
	  fprintf(stderr,"Could not malloc memory for last_malloc.\n");
	  exit(-1);
  }
  for (i=0;i<P;i++) {
    proc_bytes[i] = 0;
    last_malloc[i] = NULL;
  }
  for (i=0;i<nblocks;i++) {
    for (j=0;j<nblocks;j++) {
      proc_num = BlockOwner(i,j);
      if ((i == nblocks-1) && (j == nblocks-1)) {
        size = edge*edge;
      } else if ((i == nblocks-1) || (j == nblocks-1)) {
        size = edge*block_size;
      } else {
        size = block_size*block_size;
      }
      proc_bytes[proc_num] += size*sizeof(double);
    }
  }
  for (i=0;i<P;i++) {
    last_malloc[i] = (double *) malloc(proc_bytes[i] + PAGE_SIZE);
    if (last_malloc[i] == NULL) {
      fprintf(stderr,"Could not malloc memory blocks for proc %ld\n",i);
      exit(-1);
    } 
    last_malloc[i] = (double *) (((unsigned long) last_malloc[i]) + PAGE_SIZE -
                     ((unsigned long) last_malloc[i]) % PAGE_SIZE);

/* Note that this causes all blocks to start out page-aligned, and that
   for block sizes that exceed cache line size, blocks start at cache-line
   aligned addresses as well.  This reduces false sharing */

  }
  a = (double **) malloc(nblocks*nblocks*sizeof(double *));;
  if (a == NULL) {
    printerr("Could not malloc memory for a\n");
    exit(-1);
  } 
  for (i=0;i<nblocks;i++) {
    for (j=0;j<nblocks;j++) {
      proc_num = BlockOwner(i,j);
      a[i+j*nblocks] = last_malloc[proc_num];
      if ((i == nblocks-1) && (j == nblocks-1)) {
        size = edge*edge;
      } else if ((i == nblocks-1) || (j == nblocks-1)) {
        size = edge*block_size;
      } else {
        size = block_size*block_size;
      }
      last_malloc[proc_num] += size;
    }
  }

  rhs = (double *) malloc(n*sizeof(double));;
  if (rhs == NULL) {
    printerr("Could not malloc memory for rhs\n");
    exit(-1);
  } 

  Global = (struct GlobalMemory *) malloc(sizeof(struct GlobalMemory));;
  Global->t_in_fac = (double *) malloc(P*sizeof(double));;
  Global->t_in_mod = (double *) malloc(P*sizeof(double));;
  Global->t_in_solve = (double *) malloc(P*sizeof(double));;
  Global->t_in_bar = (double *) malloc(P*sizeof(double));;
  Global->completion = (double *) malloc(P*sizeof(double));;

  if (Global == NULL) {
    printerr("Could not malloc memory for Global\n");
    exit(-1);
  } else if (Global->t_in_fac == NULL) {
    printerr("Could not malloc memory for Global->t_in_fac\n");
    exit(-1);
  } else if (Global->t_in_mod == NULL) {
    printerr("Could not malloc memory for Global->t_in_mod\n");
    exit(-1);
  } else if (Global->t_in_solve == NULL) {
    printerr("Could not malloc memory for Global->t_in_solve\n");
    exit(-1);
  } else if (Global->t_in_bar == NULL) {
    printerr("Could not malloc memory for Global->t_in_bar\n");
    exit(-1);
  } else if (Global->completion == NULL) {
    printerr("Could not malloc memory for Global->completion\n");
    exit(-1);
  }

/* POSSIBLE ENHANCEMENT:  Here is where one might distribute the a[i]
   blocks across physically distributed memories as desired.

   One way to do this is as follows:

   for (i=0;i<nblocks;i++) {
     for (j=0;j<nblocks;j++) {
       proc_num = BlockOwner(i,j);
       if ((i == nblocks-1) && (j == nblocks-1)) {
         size = edge*edge;
       } else if ((i == nblocks-1) || (j == nblocks-1)) {
         size = edge*block_size;
       } else {
         size = block_size*block_size;
       }

       Place all addresses x such that 
       (&(a[i+j*nblocks][0]) <= x < &(a[i+j*nblocks][size-1])) 
       on node proc_num
     }
   }
*/

  {
#line 295
	unsigned long	Error;
#line 295

#line 295
	Error = pthread_mutex_init(&(Global->start).mutex, NULL);
#line 295
	if (Error != 0) {
#line 295
		printf("Error while initializing barrier.\n");
#line 295
		exit(-1);
#line 295
	}
#line 295

#line 295
	Error = pthread_cond_init(&(Global->start).cv, NULL);
#line 295
	if (Error != 0) {
#line 295
		printf("Error while initializing barrier.\n");
#line 295
		pthread_mutex_destroy(&(Global->start).mutex);
#line 295
		exit(-1);
#line 295
	}
#line 295

#line 295
	(Global->start).counter = 0;
#line 295
	(Global->start).cycle = 0;
#line 295
};
  {pthread_mutex_init(&(Global->idlock), NULL);};
  Global->id = 0;

  InitA(rhs);
  if (doprint) {
    printf("Matrix before decomposition:\n");
    PrintA();
  }

  {
#line 305
	long	i, Error;
#line 305

#line 305
	for (i = 0; i < (P) - 1; i++) {
#line 305
		Error = pthread_create(&PThreadTable[i], NULL, (void * (*)(void *))(SlaveStart), NULL);
#line 305
		if (Error != 0) {
#line 305
			printf("Error in pthread_create().\n");
#line 305
			exit(-1);
#line 305
		}
#line 305
	}
#line 305

#line 305
	SlaveStart();
#line 305
};
  {
#line 306
	unsigned long	i, Error;
#line 306
	for (i = 0; i < (P) - 1; i++) {
#line 306
		Error = pthread_join(PThreadTable[i], NULL);
#line 306
		if (Error != 0) {
#line 306
			printf("Error in pthread_join().\n");
#line 306
			exit(-1);
#line 306
		}
#line 306
	}
#line 306
};

  if (doprint) {
    printf("\nMatrix after decomposition:\n");
    PrintA();
  }

  if (dostats) {
    maxt = avgt = mint = Global->completion[0];
    for (i=1; i<P; i++) {
      if (Global->completion[i] > maxt) {
        maxt = Global->completion[i];
      }
      if (Global->completion[i] < mint) {
        mint = Global->completion[i];
      }
      avgt += Global->completion[i];
    }
    avgt = avgt / P;
  
    min_fac = max_fac = avg_fac = Global->t_in_fac[0];
    min_solve = max_solve = avg_solve = Global->t_in_solve[0];
    min_mod = max_mod = avg_mod = Global->t_in_mod[0];
    min_bar = max_bar = avg_bar = Global->t_in_bar[0];
  
    for (i=1; i<P; i++) {
      if (Global->t_in_fac[i] > max_fac) {
        max_fac = Global->t_in_fac[i];
      }
      if (Global->t_in_fac[i] < min_fac) {
        min_fac = Global->t_in_fac[i];
      }
      if (Global->t_in_solve[i] > max_solve) {
        max_solve = Global->t_in_solve[i];
      }
      if (Global->t_in_solve[i] < min_solve) {
        min_solve = Global->t_in_solve[i];
      }
      if (Global->t_in_mod[i] > max_mod) {
        max_mod = Global->t_in_mod[i];
      }
      if (Global->t_in_mod[i] < min_mod) {
        min_mod = Global->t_in_mod[i];
      }
      if (Global->t_in_bar[i] > max_bar) {
        max_bar = Global->t_in_bar[i];
      }
      if (Global->t_in_bar[i] < min_bar) {
        min_bar = Global->t_in_bar[i];
      }
      avg_fac += Global->t_in_fac[i];
      avg_solve += Global->t_in_solve[i];
      avg_mod += Global->t_in_mod[i];
      avg_bar += Global->t_in_bar[i];
    }
    avg_fac = avg_fac/P;
    avg_solve = avg_solve/P;
    avg_mod = avg_mod/P;
    avg_bar = avg_bar/P;
  }
  printf("                            PROCESS STATISTICS\n");
  printf("              Total      Diagonal     Perimeter      Interior       Barrier\n");
  printf(" Proc         Time         Time         Time           Time          Time\n");
  printf("    0    %10.0f    %10.0f    %10.0f    %10.0f    %10.0f\n",
          Global->completion[0],Global->t_in_fac[0],
          Global->t_in_solve[0],Global->t_in_mod[0],
          Global->t_in_bar[0]);
  if (dostats) {
    for (i=1; i<P; i++) {
      printf("  %3ld    %10.0f    %10.0f    %10.0f    %10.0f    %10.0f\n",
              i,Global->completion[i],Global->t_in_fac[i],
	      Global->t_in_solve[i],Global->t_in_mod[i],
	      Global->t_in_bar[i]);
    }
    printf("  Avg    %10.0f    %10.0f    %10.0f    %10.0f    %10.0f\n",
           avgt,avg_fac,avg_solve,avg_mod,avg_bar);
    printf("  Min    %10.0f    %10.0f    %10.0f    %10.0f    %10.0f\n",
           mint,min_fac,min_solve,min_mod,min_bar);
    printf("  Max    %10.0f    %10.0f    %10.0f    %10.0f    %10.0f\n",
           maxt,max_fac,max_solve,max_mod,max_bar);
  }
  printf("\n");
  Global->starttime = start;
  printf("                            TIMING INFORMATION\n");
  printf("Start time                        : %16lu\n", Global->starttime);
  printf("Initialization finish time        : %16lu\n", Global->rs);
  printf("Overall finish time               : %16lu\n", Global->rf);
  printf("Total time with initialization    : %16lu\n", Global->rf-Global->starttime);
  printf("Total time without initialization : %16lu\n", Global->rf-Global->rs);
  printf("\n");

  if (test_result) {
    printf("                             TESTING RESULTS\n");
    CheckResult(n, a, rhs);
  }

  {exit(0);};
}


void SlaveStart()
{
  long MyNum;

  {pthread_mutex_lock(&(Global->idlock));}
    MyNum = Global->id;
    Global->id ++;
  {pthread_mutex_unlock(&(Global->idlock));}

/* POSSIBLE ENHANCEMENT:  Here is where one might pin processes to
   processors to avoid migration */

  {;};
  OneSolve(n, block_size, MyNum, dostats);
}


void OneSolve(long n, long block_size, long MyNum, long dostats)
{
  unsigned long myrs; 
  unsigned long myrf; 
  unsigned long mydone;
  struct LocalCopies *lc;

  lc = (struct LocalCopies *) malloc(sizeof(struct LocalCopies));
  if (lc == NULL) {
    fprintf(stderr,"Proc %ld could not malloc memory for lc\n",MyNum);
    exit(-1);
  }
  lc->t_in_fac = 0.0;
  lc->t_in_solve = 0.0;
  lc->t_in_mod = 0.0;
  lc->t_in_bar = 0.0;

  /* barrier to ensure all initialization is done */
  {
#line 441
	unsigned long	Error, Cycle;
#line 441
	long		Cancel, Temp;
#line 441

#line 441
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 441
	if (Error != 0) {
#line 441
		printf("Error while trying to get lock in barrier.\n");
#line 441
		exit(-1);
#line 441
	}
#line 441

#line 441
	Cycle = (Global->start).cycle;
#line 441
	if (++(Global->start).counter != (P)) {
#line 441
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 441
		while (Cycle == (Global->start).cycle) {
#line 441
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 441
			if (Error != 0) {
#line 441
				break;
#line 441
			}
#line 441
		}
#line 441
		pthread_setcancelstate(Cancel, &Temp);
#line 441
	} else {
#line 441
		(Global->start).cycle = !(Global->start).cycle;
#line 441
		(Global->start).counter = 0;
#line 441
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 441
	}
#line 441
	pthread_mutex_unlock(&(Global->start).mutex);
#line 441
};

  /* to remove cold-start misses, all processors touch their own data */
  TouchA(block_size, MyNum);

  {
#line 446
	unsigned long	Error, Cycle;
#line 446
	long		Cancel, Temp;
#line 446

#line 446
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 446
	if (Error != 0) {
#line 446
		printf("Error while trying to get lock in barrier.\n");
#line 446
		exit(-1);
#line 446
	}
#line 446

#line 446
	Cycle = (Global->start).cycle;
#line 446
	if (++(Global->start).counter != (P)) {
#line 446
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 446
		while (Cycle == (Global->start).cycle) {
#line 446
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 446
			if (Error != 0) {
#line 446
				break;
#line 446
			}
#line 446
		}
#line 446
		pthread_setcancelstate(Cancel, &Temp);
#line 446
	} else {
#line 446
		(Global->start).cycle = !(Global->start).cycle;
#line 446
		(Global->start).counter = 0;
#line 446
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 446
	}
#line 446
	pthread_mutex_unlock(&(Global->start).mutex);
#line 446
};

/* POSSIBLE ENHANCEMENT:  Here is where one might reset the
   statistics that one is measuring about the parallel execution */

  if ((MyNum == 0) || (dostats)) {
    {
#line 452
	struct timeval	FullTime;
#line 452

#line 452
	gettimeofday(&FullTime, NULL);
#line 452
	(myrs) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 452
};
  }

  lu(n, block_size, MyNum, lc, dostats);

  if ((MyNum == 0) || (dostats)) {
    {
#line 458
	struct timeval	FullTime;
#line 458

#line 458
	gettimeofday(&FullTime, NULL);
#line 458
	(mydone) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 458
};
  }

  {
#line 461
	unsigned long	Error, Cycle;
#line 461
	long		Cancel, Temp;
#line 461

#line 461
	Error = pthread_mutex_lock(&(Global->start).mutex);
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
	Cycle = (Global->start).cycle;
#line 461
	if (++(Global->start).counter != (P)) {
#line 461
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 461
		while (Cycle == (Global->start).cycle) {
#line 461
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
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
		(Global->start).cycle = !(Global->start).cycle;
#line 461
		(Global->start).counter = 0;
#line 461
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 461
	}
#line 461
	pthread_mutex_unlock(&(Global->start).mutex);
#line 461
};

  if ((MyNum == 0) || (dostats)) {
    Global->t_in_fac[MyNum] = lc->t_in_fac;
    Global->t_in_solve[MyNum] = lc->t_in_solve;
    Global->t_in_mod[MyNum] = lc->t_in_mod;
    Global->t_in_bar[MyNum] = lc->t_in_bar;
    Global->completion[MyNum] = mydone-myrs;
  }
  if (MyNum == 0) {
    {
#line 471
	struct timeval	FullTime;
#line 471

#line 471
	gettimeofday(&FullTime, NULL);
#line 471
	(myrf) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 471
};
    Global->rs = myrs;
    Global->done = mydone;
    Global->rf = myrf;
  }
}


void lu0(double *a, long n, long stride)
{
  long j; 
  long k; 
  long length;
  double alpha;

  for (k=0; k<n; k++) {
    /* modify subsequent columns */
    for (j=k+1; j<n; j++) {
      a[k+j*stride] /= a[k+k*stride];
      alpha = -a[k+j*stride];
      length = n-k-1;
      daxpy(&a[k+1+j*stride], &a[k+1+k*stride], n-k-1, alpha);
    }
  }
}


void bdiv(double *a, double *diag, long stride_a, long stride_diag, long dimi, long dimk)
{
  long j; 
  long k;
  double alpha;

  for (k=0; k<dimk; k++) {
    for (j=k+1; j<dimk; j++) {
      alpha = -diag[k+j*stride_diag];
      daxpy(&a[j*stride_a], &a[k*stride_a], dimi, alpha);
    }
  }
}


void bmodd(double *a, double *c, long dimi, long dimj, long stride_a, long stride_c)
{
  long j; 
  long k; 
  long length;
  double alpha;

  for (k=0; k<dimi; k++) {
    for (j=0; j<dimj; j++) {
      c[k+j*stride_c] /= a[k+k*stride_a];
      alpha = -c[k+j*stride_c];
      length = dimi - k - 1;
      daxpy(&c[k+1+j*stride_c], &a[k+1+k*stride_a], dimi-k-1, alpha);
    }
  }
}


void bmod(double *a, double *b, double *c, long dimi, long dimj, long dimk, long stridea, long strideb, long stridec)
{
  long j; 
  long k;
  double alpha;

  for (k=0; k<dimk; k++) {
    for (j=0; j<dimj; j++) {
      alpha = -b[k+j*strideb]; 
      daxpy(&c[j*stridec], &a[k*stridea], dimi, alpha);
    }
  }
}


void daxpy(double *a, double *b, long n, double alpha)
{
  long i;

  for (i=0; i<n; i++) {
    a[i] += alpha*b[i];
  }
}


long BlockOwner(long I, long J)
{
//	return((J%num_cols) + (I%num_rows)*num_cols); 
	return((I + J) % P);
}

long BlockOwnerColumn(long I, long J)
{
	return(I % P);
}

long BlockOwnerRow(long I, long J)
{
	return(((J % P) + (P / 2)) % P);
}

void lu(long n, long bs, long MyNum, struct LocalCopies *lc, long dostats)
{
  long i, il, j, jl, k, kl;
  long I, J, K;
  double *A, *B, *C, *D;
  long strI, strJ, strK;
  unsigned long t1, t2, t3, t4, t11, t22;

  for (k=0, K=0; k<n; k+=bs, K++) {
    kl = k + bs; 
    if (kl > n) {
      kl = n;
      strK = kl - k;
    } else {
      strK = bs;
    }

    if ((MyNum == 0) || (dostats)) {
      {
#line 590
	struct timeval	FullTime;
#line 590

#line 590
	gettimeofday(&FullTime, NULL);
#line 590
	(t1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 590
};
    }

    /* factor diagonal block */
    if (BlockOwner(K, K) == MyNum) {
      A = a[K+K*nblocks]; 
      lu0(A, strK, strK);
    }

    if ((MyNum == 0) || (dostats)) {
      {
#line 600
	struct timeval	FullTime;
#line 600

#line 600
	gettimeofday(&FullTime, NULL);
#line 600
	(t11) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 600
};
    }

    {
#line 603
	unsigned long	Error, Cycle;
#line 603
	long		Cancel, Temp;
#line 603

#line 603
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 603
	if (Error != 0) {
#line 603
		printf("Error while trying to get lock in barrier.\n");
#line 603
		exit(-1);
#line 603
	}
#line 603

#line 603
	Cycle = (Global->start).cycle;
#line 603
	if (++(Global->start).counter != (P)) {
#line 603
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 603
		while (Cycle == (Global->start).cycle) {
#line 603
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 603
			if (Error != 0) {
#line 603
				break;
#line 603
			}
#line 603
		}
#line 603
		pthread_setcancelstate(Cancel, &Temp);
#line 603
	} else {
#line 603
		(Global->start).cycle = !(Global->start).cycle;
#line 603
		(Global->start).counter = 0;
#line 603
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 603
	}
#line 603
	pthread_mutex_unlock(&(Global->start).mutex);
#line 603
};

    if ((MyNum == 0) || (dostats)) {
      {
#line 606
	struct timeval	FullTime;
#line 606

#line 606
	gettimeofday(&FullTime, NULL);
#line 606
	(t2) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 606
};
    }

    /* divide column k by diagonal block */
    D = a[K+K*nblocks];
    for (i=kl, I=K+1; i<n; i+=bs, I++) {
      if (BlockOwnerColumn(I, K) == MyNum) {  /* parcel out blocks */
	il = i + bs; 
	if (il > n) {
	  il = n;
          strI = il - i;
        } else {
          strI = bs;
        }
	A = a[I+K*nblocks]; 
	bdiv(A, D, strI, strK, strI, strK);  
      }
    }
    /* modify row k by diagonal block */
    for (j=kl, J=K+1; j<n; j+=bs, J++) {
      if (BlockOwnerRow(K, J) == MyNum) {  /* parcel out blocks */
	jl = j+bs; 
	if (jl > n) {
	  jl = n;
          strJ = jl - j;
        } else {
          strJ = bs;
        }
        A = a[K+J*nblocks];
	bmodd(D, A, strK, strJ, strK, strK);
      }
    }

    if ((MyNum == 0) || (dostats)) {
      {
#line 640
	struct timeval	FullTime;
#line 640

#line 640
	gettimeofday(&FullTime, NULL);
#line 640
	(t22) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 640
};
    }   

    {
#line 643
	unsigned long	Error, Cycle;
#line 643
	long		Cancel, Temp;
#line 643

#line 643
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 643
	if (Error != 0) {
#line 643
		printf("Error while trying to get lock in barrier.\n");
#line 643
		exit(-1);
#line 643
	}
#line 643

#line 643
	Cycle = (Global->start).cycle;
#line 643
	if (++(Global->start).counter != (P)) {
#line 643
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 643
		while (Cycle == (Global->start).cycle) {
#line 643
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 643
			if (Error != 0) {
#line 643
				break;
#line 643
			}
#line 643
		}
#line 643
		pthread_setcancelstate(Cancel, &Temp);
#line 643
	} else {
#line 643
		(Global->start).cycle = !(Global->start).cycle;
#line 643
		(Global->start).counter = 0;
#line 643
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 643
	}
#line 643
	pthread_mutex_unlock(&(Global->start).mutex);
#line 643
};

    if ((MyNum == 0) || (dostats)) {
      {
#line 646
	struct timeval	FullTime;
#line 646

#line 646
	gettimeofday(&FullTime, NULL);
#line 646
	(t3) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 646
};
    }

    /* modify subsequent block columns */
    for (i=kl, I=K+1; i<n; i+=bs, I++) {
      il = i+bs; 
      if (il > n) {
	il = n;
        strI = il - i;
      } else {
        strI = bs;
      }
      A = a[I+K*nblocks]; 
      for (j=kl, J=K+1; j<n; j+=bs, J++) {
	jl = j + bs; 
	if (jl > n) {
	  jl = n;
          strJ= jl - j;
        } else {
          strJ = bs;
        }
	if (BlockOwner(I, J) == MyNum) {  /* parcel out blocks */
	  B = a[K+J*nblocks]; 
	  C = a[I+J*nblocks];
	  bmod(A, B, C, strI, strJ, strK, strI, strK, strI);
	}
      }
    }

    if ((MyNum == 0) || (dostats)) {
      {
#line 676
	struct timeval	FullTime;
#line 676

#line 676
	gettimeofday(&FullTime, NULL);
#line 676
	(t4) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 676
};
      lc->t_in_fac += (t11-t1);
      lc->t_in_solve += (t22-t2);
      lc->t_in_mod += (t4-t3);
      lc->t_in_bar += (t2-t11) + (t3-t22);
    }
  }
}


void InitA(double *rhs)
{
  long i, j;
  long ii, jj;
  long edge;
  long ibs;
  long jbs, skip;

  srand48((long) 1);
  edge = n%block_size;
  for (j=0; j<n; j++) {
    for (i=0; i<n; i++) {
      if ((n - i) <= edge) {
	ibs = edge;
	ibs = n-edge;
	skip = edge;
      } else {
	ibs = block_size;
	skip = block_size;
      }
      if ((n - j) <= edge) {
	jbs = edge;
	jbs = n-edge;
      } else {
	jbs = block_size;
      }
      ii = (i/block_size) + (j/block_size)*nblocks;
      jj = (i%ibs)+(j%jbs)*skip;
      a[ii][jj] = ((double) lrand48())/MAXRAND;
      if (i == j) {
	a[ii][jj] *= 10;
      }
    }
  }

  for (j=0; j<n; j++) {
    rhs[j] = 0.0;
  }
  for (j=0; j<n; j++) {
    for (i=0; i<n; i++) {
      if ((n - i) <= edge) {
	ibs = edge;
	ibs = n-edge;
	skip = edge;
      } else {
	ibs = block_size;
	skip = block_size;
      }
      if ((n - j) <= edge) {
	jbs = edge;
	jbs = n-edge;
      } else {
	jbs = block_size;
      }
      ii = (i/block_size) + (j/block_size)*nblocks;
      jj = (i%ibs)+(j%jbs)*skip;
      rhs[i] += a[ii][jj];
    }
  }
}


double TouchA(long bs, long MyNum)
{
  long i, j, I, J;
  double tot = 0.0;
  long ibs;
  long jbs;

  /* touch my portion of A[] */

  for (J=0; J<nblocks; J++) {
    for (I=0; I<nblocks; I++) {
      if (BlockOwner(I, J) == MyNum) {
	if (J == nblocks-1) {
	  jbs = n%bs;
	  if (jbs == 0) {
	    jbs = bs;
          }
	} else {
	  jbs = bs;
	}
	if (I == nblocks-1) {
	  ibs = n%bs;
	  if (ibs == 0) {
	    ibs = bs;
          }
	} else {
	  ibs = bs;
	}
	for (j=0; j<jbs; j++) {
	  for (i=0; i<ibs; i++) {
	    tot += a[I+J*nblocks][i+j*ibs];
          }
	}
      }
    }
  } 
  return(tot);
}


void PrintA()
{
  long i, j;
  long ii, jj;
  long edge;
  long ibs, jbs, skip;

  edge = n%block_size;
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      if ((n - i) <= edge) {
	ibs = edge;
	ibs = n-edge;
        skip = edge;
      } else {
	ibs = block_size;
        skip = block_size;
      }
      if ((n - j) <= edge) {
	jbs = edge;
	jbs = n-edge;
      } else {
	jbs = block_size;
      }
      ii = (i/block_size) + (j/block_size)*nblocks;
      jj = (i%ibs)+(j%jbs)*skip;
      printf("%8.1f ", a[ii][jj]);   
    }
    printf("\n");
  }
  fflush(stdout);
}


void CheckResult(long n, double **a, double *rhs)
{
  long i, j, bogus = 0;
  double *y, diff, max_diff;
  long ii, jj;
  long edge;
  long ibs, jbs, skip;

  edge = n%block_size;
  y = (double *) malloc(n*sizeof(double));  
  if (y == NULL) {
    printerr("Could not malloc memory for y\n");
    exit(-1);
  }
  for (j=0; j<n; j++) {
    y[j] = rhs[j];
  }
  for (j=0; j<n; j++) {
    if ((n - j) <= edge) {
      jbs = edge;
      jbs = n-edge;
      skip = edge;
    } else {
      jbs = block_size;
      skip = block_size;
    }
    ii = (j/block_size) + (j/block_size)*nblocks;
    jj = (j%jbs)+(j%jbs)*skip;

    y[j] = y[j]/a[ii][jj];
    for (i=j+1; i<n; i++) {
      if ((n - i) <= edge) {
        ibs = edge;
        ibs = n-edge;
        skip = edge;
      } else {
        ibs = block_size;
        skip = block_size;
      }
      ii = (i/block_size) + (j/block_size)*nblocks;
      jj = (i%ibs)+(j%jbs)*skip;

      y[i] -= a[ii][jj]*y[j];
    }
  }

  for (j=n-1; j>=0; j--) {
    for (i=0; i<j; i++) {
      if ((n - i) <= edge) {
	ibs = edge;
        ibs = n-edge;
        skip = edge;
      } else {
	ibs = block_size;
        skip = block_size;
      }
      if ((n - j) <= edge) {
	jbs = edge;
        jbs = n-edge;
      } else {
	jbs = block_size;
      }
      ii = (i/block_size) + (j/block_size)*nblocks;
      jj = (i%ibs)+(j%jbs)*skip;
      y[i] -= a[ii][jj]*y[j];
    }
  }

  max_diff = 0.0;
  for (j=0; j<n; j++) {
    diff = y[j] - 1.0;
    if (fabs(diff) > 0.00001) {
      bogus = 1;
      max_diff = diff;
    }
  }
  if (bogus) {
    printf("TEST FAILED: (%.5f diff)\n", max_diff);
  } else {
    printf("TEST PASSED\n");
  }
  free(y);
}


void printerr(char *s)
{
  fprintf(stderr,"ERROR: %s\n",s);
}

