#line 228 "../null_macros/c.m4.null"

#line 1 "decs.H"
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

#define INPROCS        16
#define IMAX          258
#define JMAX          258
#define MAX_LEVELS      9
#define MASTER          0
#define RED_ITER        0
#define BLACK_ITER      1
#define PAGE_SIZE    4096


#line 26
#include <pthread.h>
#line 26
#include <sys/time.h>
#line 26
#include <unistd.h>
#line 26
#include <stdlib.h>
#line 26
extern pthread_t PThreadTable[];
#line 26


struct global_struct {
   long id;
   long starttime;
   long trackstart;
   double psiai;
   double psibi;
};

struct fields_struct {
   double psi[2][IMAX][JMAX];
   double psim[2][IMAX][JMAX];
};

struct fields2_struct {
   double psium[IMAX][JMAX];
   double psilm[IMAX][JMAX];
};

struct wrk1_struct {
   double psib[IMAX][JMAX];
   double ga[IMAX][JMAX];
   double gb[IMAX][JMAX];
};

struct wrk3_struct {
   double work1[2][IMAX][JMAX];
   double work2[IMAX][JMAX];
};

struct wrk2_struct {
   double work3[IMAX][JMAX];
   double f[IMAX];
};

struct wrk4_struct {
   double work4[2][IMAX][JMAX];
   double work5[2][IMAX][JMAX];
};

struct wrk6_struct {
   double work6[IMAX][JMAX];
};

struct wrk5_struct {
   double work7[2][IMAX][JMAX];
   double temparray[2][IMAX][JMAX];
};

struct frcng_struct {
   double tauz[IMAX][JMAX];
};

struct iter_struct {
   long notdone;
   double work8[IMAX][JMAX];
   double work9[IMAX][JMAX];
};

struct guess_struct {
   double oldga[IMAX][JMAX];
   double oldgb[IMAX][JMAX];
};

struct multi_struct {
   double q_multi[MAX_LEVELS][IMAX][JMAX];
   double rhs_multi[MAX_LEVELS][IMAX][JMAX];
   double err_multi;
   long numspin;
   long spinflag[INPROCS];
};

struct locks_struct {
   pthread_mutex_t (idlock);
   pthread_mutex_t (psiailock);
   pthread_mutex_t (psibilock);
   pthread_mutex_t (donelock);
   pthread_mutex_t (error_lock);
   pthread_mutex_t (bar_lock);
};

struct bars_struct {
#if defined(MULTIPLE_BARRIERS)
   
#line 110
struct {
#line 110
	pthread_mutex_t	mutex;
#line 110
	pthread_cond_t	cv;
#line 110
	unsigned long	counter;
#line 110
	unsigned long	cycle;
#line 110
} (iteration);
#line 110

   
#line 111
struct {
#line 111
	pthread_mutex_t	mutex;
#line 111
	pthread_cond_t	cv;
#line 111
	unsigned long	counter;
#line 111
	unsigned long	cycle;
#line 111
} (gsudn);
#line 111

   
#line 112
struct {
#line 112
	pthread_mutex_t	mutex;
#line 112
	pthread_cond_t	cv;
#line 112
	unsigned long	counter;
#line 112
	unsigned long	cycle;
#line 112
} (p_setup);
#line 112

   
#line 113
struct {
#line 113
	pthread_mutex_t	mutex;
#line 113
	pthread_cond_t	cv;
#line 113
	unsigned long	counter;
#line 113
	unsigned long	cycle;
#line 113
} (p_redph);
#line 113

   
#line 114
struct {
#line 114
	pthread_mutex_t	mutex;
#line 114
	pthread_cond_t	cv;
#line 114
	unsigned long	counter;
#line 114
	unsigned long	cycle;
#line 114
} (p_soln);
#line 114

   
#line 115
struct {
#line 115
	pthread_mutex_t	mutex;
#line 115
	pthread_cond_t	cv;
#line 115
	unsigned long	counter;
#line 115
	unsigned long	cycle;
#line 115
} (p_subph);
#line 115

   
#line 116
struct {
#line 116
	pthread_mutex_t	mutex;
#line 116
	pthread_cond_t	cv;
#line 116
	unsigned long	counter;
#line 116
	unsigned long	cycle;
#line 116
} (sl_prini);
#line 116

   
#line 117
struct {
#line 117
	pthread_mutex_t	mutex;
#line 117
	pthread_cond_t	cv;
#line 117
	unsigned long	counter;
#line 117
	unsigned long	cycle;
#line 117
} (sl_psini);
#line 117

   
#line 118
struct {
#line 118
	pthread_mutex_t	mutex;
#line 118
	pthread_cond_t	cv;
#line 118
	unsigned long	counter;
#line 118
	unsigned long	cycle;
#line 118
} (sl_onetime);
#line 118

   
#line 119
struct {
#line 119
	pthread_mutex_t	mutex;
#line 119
	pthread_cond_t	cv;
#line 119
	unsigned long	counter;
#line 119
	unsigned long	cycle;
#line 119
} (sl_phase_1);
#line 119

   
#line 120
struct {
#line 120
	pthread_mutex_t	mutex;
#line 120
	pthread_cond_t	cv;
#line 120
	unsigned long	counter;
#line 120
	unsigned long	cycle;
#line 120
} (sl_phase_2);
#line 120

   
#line 121
struct {
#line 121
	pthread_mutex_t	mutex;
#line 121
	pthread_cond_t	cv;
#line 121
	unsigned long	counter;
#line 121
	unsigned long	cycle;
#line 121
} (sl_phase_3);
#line 121

   
#line 122
struct {
#line 122
	pthread_mutex_t	mutex;
#line 122
	pthread_cond_t	cv;
#line 122
	unsigned long	counter;
#line 122
	unsigned long	cycle;
#line 122
} (sl_phase_4);
#line 122

   
#line 123
struct {
#line 123
	pthread_mutex_t	mutex;
#line 123
	pthread_cond_t	cv;
#line 123
	unsigned long	counter;
#line 123
	unsigned long	cycle;
#line 123
} (sl_phase_5);
#line 123

   
#line 124
struct {
#line 124
	pthread_mutex_t	mutex;
#line 124
	pthread_cond_t	cv;
#line 124
	unsigned long	counter;
#line 124
	unsigned long	cycle;
#line 124
} (sl_phase_6);
#line 124

   
#line 125
struct {
#line 125
	pthread_mutex_t	mutex;
#line 125
	pthread_cond_t	cv;
#line 125
	unsigned long	counter;
#line 125
	unsigned long	cycle;
#line 125
} (sl_phase_7);
#line 125

   
#line 126
struct {
#line 126
	pthread_mutex_t	mutex;
#line 126
	pthread_cond_t	cv;
#line 126
	unsigned long	counter;
#line 126
	unsigned long	cycle;
#line 126
} (sl_phase_8);
#line 126

   
#line 127
struct {
#line 127
	pthread_mutex_t	mutex;
#line 127
	pthread_cond_t	cv;
#line 127
	unsigned long	counter;
#line 127
	unsigned long	cycle;
#line 127
} (sl_phase_9);
#line 127

   
#line 128
struct {
#line 128
	pthread_mutex_t	mutex;
#line 128
	pthread_cond_t	cv;
#line 128
	unsigned long	counter;
#line 128
	unsigned long	cycle;
#line 128
} (sl_phase_10);
#line 128

   
#line 129
struct {
#line 129
	pthread_mutex_t	mutex;
#line 129
	pthread_cond_t	cv;
#line 129
	unsigned long	counter;
#line 129
	unsigned long	cycle;
#line 129
} (error_barrier);
#line 129

#else
   
#line 131
struct {
#line 131
	pthread_mutex_t	mutex;
#line 131
	pthread_cond_t	cv;
#line 131
	unsigned long	counter;
#line 131
	unsigned long	cycle;
#line 131
} (barrier);
#line 131

#endif
};

extern struct global_struct *global;
extern struct fields_struct *fields;
extern struct fields2_struct *fields2;
extern struct wrk1_struct *wrk1;
extern struct wrk3_struct *wrk3;
extern struct wrk2_struct *wrk2;
extern struct wrk4_struct *wrk4;
extern struct wrk6_struct *wrk6;
extern struct wrk5_struct *wrk5;
extern struct frcng_struct *frcng;
extern struct iter_struct *iter;
extern struct guess_struct *guess;
extern struct multi_struct *multi;
extern struct locks_struct *locks;
extern struct bars_struct *bars;

extern double eig2;
extern double ysca;
extern long jmm1;
extern double pi;
extern double t0;

extern long *procmap;
extern long xprocs;
extern long yprocs;

extern long numlev;
extern long imx[MAX_LEVELS];
extern long jmx[MAX_LEVELS];
extern double lev_res[MAX_LEVELS];
extern double lev_tol[MAX_LEVELS];
extern double maxwork;
extern long minlevel;
extern double outday0;
extern double outday1;
extern double outday2;
extern double outday3;

extern long nprocs;

extern double h1;
extern double h3;
extern double h;
extern double lf;
extern double res;
extern double dtau;
extern double f0;
extern double beta;
extern double gpr;
extern long im;
extern long jm;
extern long do_stats;
extern long do_output;
extern long *multi_times;
extern long *total_times;
extern double factjacob;
extern double factlap;

struct Global_Private {
  char pad[PAGE_SIZE];
  double multi_time;
  double total_time;
  long rel_start_x[MAX_LEVELS];
  long rel_start_y[MAX_LEVELS];
  long rel_num_x[MAX_LEVELS];
  long rel_num_y[MAX_LEVELS];
  long eist[MAX_LEVELS];
  long ejst[MAX_LEVELS];
  long oist[MAX_LEVELS];
  long ojst[MAX_LEVELS];
  long eiest[MAX_LEVELS];
  long ejest[MAX_LEVELS];
  long oiest[MAX_LEVELS];
  long ojest[MAX_LEVELS];
  long rlist[MAX_LEVELS];
  long rljst[MAX_LEVELS];
  long rlien[MAX_LEVELS];
  long rljen[MAX_LEVELS];
  long iist[MAX_LEVELS];
  long ijst[MAX_LEVELS];
  long iien[MAX_LEVELS];
  long ijen[MAX_LEVELS];
  long pist[MAX_LEVELS];
  long pjst[MAX_LEVELS];
  long pien[MAX_LEVELS];
  long pjen[MAX_LEVELS];
};

extern struct Global_Private *gp;

extern double i_int_coeff[MAX_LEVELS];
extern double j_int_coeff[MAX_LEVELS];
extern long minlev;

/*
 * jacobcalc.C
 */
void jacobcalc(double x[IMAX][JMAX], double y[IMAX][JMAX], double z[IMAX][JMAX], long pid, long firstrow, long lastrow, long firstcol, long lastcol, long numrows, long numcols);

/*
 * laplacalc.C
 */
void laplacalc(double x[IMAX][JMAX], double z[IMAX][JMAX], long firstrow, long lastrow, long firstcol, long lastcol, long numrows, long numcols);

/*
 * main.C
 */
long log_2(long number);
void printerr(char *s);

/*
 * multi.C
 */
void multig(long my_id);
void relax(long k, double *err, long color, long my_num);
void rescal(long kf, long my_num);
void intadd(long kc, long my_num);
void putz(long k, long my_num);

/*
 * slave1.C
 */
void slave(void);

/*
 * slave2.C
 */
void slave2(long procid, long firstrow, long lastrow, long numrows, long firstcol, long lastcol, long numcols);
