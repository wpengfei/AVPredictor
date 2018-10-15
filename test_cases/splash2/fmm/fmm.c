#line 228 "../null_macros/c.m4.null"

#line 1 "fmm.C"
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

/*
 *  FMM.C
 *
 *    This file contains the entry to Greengard's adaptive algorithm.
 *

    Usage: FMM <options> < inputfile

    Command line options:

      -o : Print out final particle positions.
      -s : Print out individual processor timing statistics.
      -h : Print out command line options

    Input file parameter description:
      There are a total of nine parameters, with parameters
      three through seven having no default values.

      1) Cluster Type : Particles are distributed either in one cluster,
         or two interacting clusters of size (# of particles)/ 2.
         These two options are selected by the strings "one cluster" or
         "two cluster". The default is for two clusters.
      2) Distribution Type : Particles are distributed in a cluster
         either in a spherical uniform distribution, or according to
         the Plummer model which typically has a large percentage of the
         particles close to the center of the sphere and fewer particles
         farther from the center.  There two options are selected by
         the strings "uniform" or "plummer". The default is for a
         plummer distribution.
      3) Number Of Particles : Should be an integer greater than 0.
      4) Precision : A measure of how accurate the calculation should be.
         A precision of 1e-3 means that the results will be accurate to
         within three decimal places regardless of the relative magnitude
         of the positions.  The precision should be a real number greater
         than 0.
      5) Number of Processors : Should be an integer greater than 0.
      6) Number of Time Steps : Should be an integer greater than 0.
      7) Duration of a Time Step : How long each time step lasts.
         Should be a double greater than 0.
      8) Softening Parameter : This value sets the minimum distance in
         each direction that two particles can be separated by.  If two
         particles are closer than this, the distance used for the
         calculation is changed to the softening parameter. The particle
         positions themselves are NOT changed. This number should be a
         real number greater than 0 and defaults to DBL_MIN or FLT_MIN,
         depending on what type of data is being used.
      9) Partitioning Scheme : Sets which type of partitioning scheme
         is used. There are currently two : "cost zones" and "orb".
         The default is cost zones.

 */

#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "memory.h"
#include "particle.h"
#include "box.h"
#include "partition_grid.h"
#include "cost_zones.h"
#include "construct_grid.h"
#include "interactions.h"

#define BASE ((((double) 4) - sqrt((double) 2)) / sqrt((double) 2))
#define MAX_LINE_SIZE 100
/* OCCUPANCY * maximum particles per box = avg number of particles per box */
#define OCCUPANCY ((MAX_PARTICLES_PER_BOX > 5) ? .375 : .750)
/* Some processors will be given more than the average number of particles.
 * PDF (Particle Distribution Factor) is the ratio of the maximum to the avg */
#define PDF 4.0
/* A nonuniform distribution will require more boxes than a uniform
 * distribution of the same size. TOLERANCE is used to account for this */
#define TOLERANCE 1.5
/* Save as PDF, but for boxes */
/* define BDF (((Total_Particles/Number_Of_Processors) > 128) ? 2.0 : 3.0)*/
#define BDF (((Total_Particles/Number_Of_Processors) > 128) ? 4.0 : 8.0)

static partition_alg Partition_Flag;
static real Precision;
static long Time_Steps;
static cluster_type Cluster;
static model_type Model;
long do_stats = 0;
long do_output = 0;
unsigned long starttime;
unsigned long endtime;

void ParallelExecute(void);
void StepSimulation(long my_id, time_info *local_time, long time_all);
void PartitionGrid(long my_id, time_info *local_time, long time_all);
void GetArguments(void);
void PrintTimes(void);
void Help(void);


int
main (int argc, char *argv[])
{
   long c;
   extern char *optarg;

   {
#line 121
	struct timeval	FullTime;
#line 121

#line 121
	gettimeofday(&FullTime, NULL);
#line 121
	(starttime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 121
};

   while ((c = getopt(argc, argv, "osh")) != -1) {
     switch(c) {
       case 'o': do_output = 1; break;
       case 's': do_stats = 1; break;
       case 'h': Help(); break;
     }
   }

   {;};

   GetArguments();
   InitGlobalMemory();
   InitExpTables();
   CreateDistribution(Cluster, Model);

/*   for (i = 1; i < Number_Of_Processors; i++) {
      {
#line 139
	long	i, Error;
#line 139

#line 139
	for (i = 0; i < () - 1; i++) {
#line 139
		Error = pthread_create(&PThreadTable[i], NULL, (void * (*)(void *))(ParallelExecute), NULL);
#line 139
		if (Error != 0) {
#line 139
			printf("Error in pthread_create().\n");
#line 139
			exit(-1);
#line 139
		}
#line 139
	}
#line 139

#line 139
	ParallelExecute();
#line 139
};
   }
   ParallelExecute();
   {
#line 142
	unsigned long	i, Error;
#line 142
	for (i = 0; i < (Number_Of_Processors - 1) - 1; i++) {
#line 142
		Error = pthread_join(PThreadTable[i], NULL);
#line 142
		if (Error != 0) {
#line 142
			printf("Error in pthread_join().\n");
#line 142
			exit(-1);
#line 142
		}
#line 142
	}
#line 142
};*/
   {
#line 143
	long	i, Error;
#line 143

#line 143
	for (i = 0; i < (Number_Of_Processors) - 1; i++) {
#line 143
		Error = pthread_create(&PThreadTable[i], NULL, (void * (*)(void *))(ParallelExecute), NULL);
#line 143
		if (Error != 0) {
#line 143
			printf("Error in pthread_create().\n");
#line 143
			exit(-1);
#line 143
		}
#line 143
	}
#line 143

#line 143
	ParallelExecute();
#line 143
};
   {
#line 144
	unsigned long	i, Error;
#line 144
	for (i = 0; i < (Number_Of_Processors) - 1; i++) {
#line 144
		Error = pthread_join(PThreadTable[i], NULL);
#line 144
		if (Error != 0) {
#line 144
			printf("Error in pthread_join().\n");
#line 144
			exit(-1);
#line 144
		}
#line 144
	}
#line 144
};

   printf("Finished FMM\n");
   PrintTimes();
   if (do_output) {
     PrintAllParticles();
   }
   {exit(0);};
}


void
ParallelExecute ()
{
   long my_id;
   long num_boxes;
   unsigned long start, finish = 0;
   time_info *local_time;
   long time_all = 0;
   time_info *timing;
   unsigned long local_init_done = 0;

   {;};
   local_time = (time_info *) malloc(sizeof(struct _Time_Info) * MAX_TIME_STEPS);
   {
#line 168
	unsigned long	Error, Cycle;
#line 168
	long		Cancel, Temp;
#line 168

#line 168
	Error = pthread_mutex_lock(&(G_Memory->synch).mutex);
#line 168
	if (Error != 0) {
#line 168
		printf("Error while trying to get lock in barrier.\n");
#line 168
		exit(-1);
#line 168
	}
#line 168

#line 168
	Cycle = (G_Memory->synch).cycle;
#line 168
	if (++(G_Memory->synch).counter != (Number_Of_Processors)) {
#line 168
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 168
		while (Cycle == (G_Memory->synch).cycle) {
#line 168
			Error = pthread_cond_wait(&(G_Memory->synch).cv, &(G_Memory->synch).mutex);
#line 168
			if (Error != 0) {
#line 168
				break;
#line 168
			}
#line 168
		}
#line 168
		pthread_setcancelstate(Cancel, &Temp);
#line 168
	} else {
#line 168
		(G_Memory->synch).cycle = !(G_Memory->synch).cycle;
#line 168
		(G_Memory->synch).counter = 0;
#line 168
		Error = pthread_cond_broadcast(&(G_Memory->synch).cv);
#line 168
	}
#line 168
	pthread_mutex_unlock(&(G_Memory->synch).mutex);
#line 168
};
   {pthread_mutex_lock(&(G_Memory->count_lock));};
     my_id = G_Memory->id;
     G_Memory->id++;
   {pthread_mutex_unlock(&(G_Memory->count_lock));};

/* POSSIBLE ENHANCEMENT:  Here is where one might pin processes to
   processors to avoid migration */

   if (my_id == 0) {
     time_all = 1;
   } else if (do_stats) {
     time_all = 1;
   }

   if (my_id == 0) {
      /* have to allocate extra space since it will construct the grid by
       * itself for the first time step */
      CreateParticleList(my_id, Total_Particles);
      InitParticleList(my_id, Total_Particles, 0);
   }
   else {
      CreateParticleList(my_id, ((Total_Particles * PDF)
				 / Number_Of_Processors));
      InitParticleList(my_id, 0, 0);
   }
   num_boxes = 1.333 * (Total_Particles / (OCCUPANCY * MAX_PARTICLES_PER_BOX));
   if (my_id == 0)
      CreateBoxes(my_id, TOLERANCE * num_boxes);
   else
      CreateBoxes(my_id, TOLERANCE * num_boxes * BDF / Number_Of_Processors);

   if (my_id == 0) {
      LockedPrint("Starting FMM with %d processor%s\n", Number_Of_Processors,
		  (Number_Of_Processors == 1) ? "" : "s");
   }
   {
#line 204
	unsigned long	Error, Cycle;
#line 204
	long		Cancel, Temp;
#line 204

#line 204
	Error = pthread_mutex_lock(&(G_Memory->synch).mutex);
#line 204
	if (Error != 0) {
#line 204
		printf("Error while trying to get lock in barrier.\n");
#line 204
		exit(-1);
#line 204
	}
#line 204

#line 204
	Cycle = (G_Memory->synch).cycle;
#line 204
	if (++(G_Memory->synch).counter != (Number_Of_Processors)) {
#line 204
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 204
		while (Cycle == (G_Memory->synch).cycle) {
#line 204
			Error = pthread_cond_wait(&(G_Memory->synch).cv, &(G_Memory->synch).mutex);
#line 204
			if (Error != 0) {
#line 204
				break;
#line 204
			}
#line 204
		}
#line 204
		pthread_setcancelstate(Cancel, &Temp);
#line 204
	} else {
#line 204
		(G_Memory->synch).cycle = !(G_Memory->synch).cycle;
#line 204
		(G_Memory->synch).counter = 0;
#line 204
		Error = pthread_cond_broadcast(&(G_Memory->synch).cv);
#line 204
	}
#line 204
	pthread_mutex_unlock(&(G_Memory->synch).mutex);
#line 204
};
   Local[my_id].Time = 0.0;
   for (MY_TIME_STEP = 0; MY_TIME_STEP < Time_Steps; MY_TIME_STEP++) {

      if (MY_TIME_STEP == 2) {
/* POSSIBLE ENHANCEMENT:  Here is where one might reset the
   statistics that one is measuring about the parallel execution */
      }

      if (MY_TIME_STEP == 2) {
        if (do_stats || my_id == 0) {
          {
#line 215
	struct timeval	FullTime;
#line 215

#line 215
	gettimeofday(&FullTime, NULL);
#line 215
	(local_init_done) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 215
};
        }
      }

      if (MY_TIME_STEP == 0) {
	 {
#line 220
	struct timeval	FullTime;
#line 220

#line 220
	gettimeofday(&FullTime, NULL);
#line 220
	(start) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 220
};
      }
      else
	 start = finish;
      ConstructGrid(my_id,local_time,time_all);
      ConstructLists(my_id,local_time,time_all);
      PartitionGrid(my_id,local_time,time_all);
      StepSimulation(my_id,local_time,time_all);
      DestroyGrid(my_id,local_time,time_all);
      {
#line 229
	struct timeval	FullTime;
#line 229

#line 229
	gettimeofday(&FullTime, NULL);
#line 229
	(finish) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 229
};
      Local[my_id].Time += Timestep_Dur;
      MY_TIMING[MY_TIME_STEP].total_time = finish - start;
   }
   if (my_id == 0) {
      {
#line 234
	struct timeval	FullTime;
#line 234

#line 234
	gettimeofday(&FullTime, NULL);
#line 234
	(endtime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 234
};
   }
   {
#line 236
	unsigned long	Error, Cycle;
#line 236
	long		Cancel, Temp;
#line 236

#line 236
	Error = pthread_mutex_lock(&(G_Memory->synch).mutex);
#line 236
	if (Error != 0) {
#line 236
		printf("Error while trying to get lock in barrier.\n");
#line 236
		exit(-1);
#line 236
	}
#line 236

#line 236
	Cycle = (G_Memory->synch).cycle;
#line 236
	if (++(G_Memory->synch).counter != (Number_Of_Processors)) {
#line 236
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 236
		while (Cycle == (G_Memory->synch).cycle) {
#line 236
			Error = pthread_cond_wait(&(G_Memory->synch).cv, &(G_Memory->synch).mutex);
#line 236
			if (Error != 0) {
#line 236
				break;
#line 236
			}
#line 236
		}
#line 236
		pthread_setcancelstate(Cancel, &Temp);
#line 236
	} else {
#line 236
		(G_Memory->synch).cycle = !(G_Memory->synch).cycle;
#line 236
		(G_Memory->synch).counter = 0;
#line 236
		Error = pthread_cond_broadcast(&(G_Memory->synch).cv);
#line 236
	}
#line 236
	pthread_mutex_unlock(&(G_Memory->synch).mutex);
#line 236
};
   for (MY_TIME_STEP = 0; MY_TIME_STEP < Time_Steps; MY_TIME_STEP++) {
     timing = &(MY_TIMING[MY_TIME_STEP]);
     timing->other_time = local_time[MY_TIME_STEP].other_time;
     timing->construct_time = local_time[MY_TIME_STEP].construct_time;
     timing->list_time = local_time[MY_TIME_STEP].list_time;
     timing->partition_time = local_time[MY_TIME_STEP].partition_time;
     timing->pass_time = local_time[MY_TIME_STEP].pass_time;
     timing->inter_time = local_time[MY_TIME_STEP].inter_time;
     timing->barrier_time = local_time[MY_TIME_STEP].barrier_time;
     timing->intra_time = local_time[MY_TIME_STEP].intra_time;
   }
   Local[my_id].init_done_times = local_init_done;
   {
#line 249
	unsigned long	Error, Cycle;
#line 249
	long		Cancel, Temp;
#line 249

#line 249
	Error = pthread_mutex_lock(&(G_Memory->synch).mutex);
#line 249
	if (Error != 0) {
#line 249
		printf("Error while trying to get lock in barrier.\n");
#line 249
		exit(-1);
#line 249
	}
#line 249

#line 249
	Cycle = (G_Memory->synch).cycle;
#line 249
	if (++(G_Memory->synch).counter != (Number_Of_Processors)) {
#line 249
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 249
		while (Cycle == (G_Memory->synch).cycle) {
#line 249
			Error = pthread_cond_wait(&(G_Memory->synch).cv, &(G_Memory->synch).mutex);
#line 249
			if (Error != 0) {
#line 249
				break;
#line 249
			}
#line 249
		}
#line 249
		pthread_setcancelstate(Cancel, &Temp);
#line 249
	} else {
#line 249
		(G_Memory->synch).cycle = !(G_Memory->synch).cycle;
#line 249
		(G_Memory->synch).counter = 0;
#line 249
		Error = pthread_cond_broadcast(&(G_Memory->synch).cv);
#line 249
	}
#line 249
	pthread_mutex_unlock(&(G_Memory->synch).mutex);
#line 249
};
}


void
PartitionGrid (long my_id, time_info *local_time, long time_all)
{
   unsigned long start = 0, finish;

   if (time_all)
      {
#line 259
	struct timeval	FullTime;
#line 259

#line 259
	gettimeofday(&FullTime, NULL);
#line 259
	(start) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 259
};
   if (Partition_Flag == COST_ZONES)
      CostZones(my_id);
   if (time_all) {
      {
#line 263
	struct timeval	FullTime;
#line 263

#line 263
	gettimeofday(&FullTime, NULL);
#line 263
	(finish) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 263
};
      local_time[MY_TIME_STEP].partition_time = finish - start;
   }
}


void
StepSimulation (long my_id, time_info *local_time, long time_all)
{
   unsigned long start, finish;
   unsigned long upward_end, interaction_end, downward_end, barrier_end;

   if (time_all)
      {
#line 276
	struct timeval	FullTime;
#line 276

#line 276
	gettimeofday(&FullTime, NULL);
#line 276
	(start) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 276
};
   PartitionIterate(my_id, UpwardPass, BOTTOM);
   if (time_all)
      {
#line 279
	struct timeval	FullTime;
#line 279

#line 279
	gettimeofday(&FullTime, NULL);
#line 279
	(upward_end) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 279
};
   PartitionIterate(my_id, ComputeInteractions, BOTTOM);
   if (time_all)
      {
#line 282
	struct timeval	FullTime;
#line 282

#line 282
	gettimeofday(&FullTime, NULL);
#line 282
	(interaction_end) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 282
};
   {
#line 283
	unsigned long	Error, Cycle;
#line 283
	long		Cancel, Temp;
#line 283

#line 283
	Error = pthread_mutex_lock(&(G_Memory->synch).mutex);
#line 283
	if (Error != 0) {
#line 283
		printf("Error while trying to get lock in barrier.\n");
#line 283
		exit(-1);
#line 283
	}
#line 283

#line 283
	Cycle = (G_Memory->synch).cycle;
#line 283
	if (++(G_Memory->synch).counter != (Number_Of_Processors)) {
#line 283
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 283
		while (Cycle == (G_Memory->synch).cycle) {
#line 283
			Error = pthread_cond_wait(&(G_Memory->synch).cv, &(G_Memory->synch).mutex);
#line 283
			if (Error != 0) {
#line 283
				break;
#line 283
			}
#line 283
		}
#line 283
		pthread_setcancelstate(Cancel, &Temp);
#line 283
	} else {
#line 283
		(G_Memory->synch).cycle = !(G_Memory->synch).cycle;
#line 283
		(G_Memory->synch).counter = 0;
#line 283
		Error = pthread_cond_broadcast(&(G_Memory->synch).cv);
#line 283
	}
#line 283
	pthread_mutex_unlock(&(G_Memory->synch).mutex);
#line 283
};
   if (time_all)
      {
#line 285
	struct timeval	FullTime;
#line 285

#line 285
	gettimeofday(&FullTime, NULL);
#line 285
	(barrier_end) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 285
};
   PartitionIterate(my_id, DownwardPass, TOP);
   if (time_all)
      {
#line 288
	struct timeval	FullTime;
#line 288

#line 288
	gettimeofday(&FullTime, NULL);
#line 288
	(downward_end) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 288
};
   PartitionIterate(my_id, ComputeParticlePositions, CHILDREN);
   if (time_all)
      {
#line 291
	struct timeval	FullTime;
#line 291

#line 291
	gettimeofday(&FullTime, NULL);
#line 291
	(finish) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 291
};

   if (time_all) {
      local_time[MY_TIME_STEP].pass_time = upward_end - start;
      local_time[MY_TIME_STEP].inter_time = interaction_end - upward_end;
      local_time[MY_TIME_STEP].barrier_time = barrier_end - interaction_end;
      local_time[MY_TIME_STEP].pass_time += downward_end - barrier_end;
      local_time[MY_TIME_STEP].intra_time = finish - downward_end;
   }
}


void
GetArguments ()
{
   char *input;

   input = (char *) malloc(MAX_LINE_SIZE * sizeof(char));
   if (input == NULL) {
      fprintf(stderr, "ERROR\n");
      exit(-1);
   }
   gets(input);
   if (strcmp(input, "one cluster") == 0)
      Cluster = ONE_CLUSTER;
   else {
      if ((*input == '\0') || (strcmp(input, "two cluster") == 0))
	 Cluster = TWO_CLUSTER;
      else {
	 fprintf(stderr, "ERROR: The only cluster types available are ");
	 fprintf(stderr, "\"one cluster\" or \"two cluster\".\n");
	 fprintf(stderr, "If you need help, type \"nbody -help\".\n");
	 exit(-1);
      }
   }

   gets(input);
   if (strcmp(input, "uniform") == 0)
      Model = UNIFORM;
   else {
      if ((*input == '\0') || (strcmp(input, "plummer") == 0))
	 Model = PLUMMER;
      else {
	 fprintf(stderr, "ERROR: The only distributions available are ");
	 fprintf(stderr, "\"uniform\" or \"plummer\".\n");
	 fprintf(stderr, "If you need help, type \"nbody -help\".\n");
	 exit(-1);
      }
   }

   Total_Particles = atoi(gets(input));
   if (Total_Particles <= 0) {
      fprintf(stderr, "ERROR: The number of particles should be an int ");
      fprintf(stderr, "greater than 0.\n");
      fprintf(stderr, "If you need help, type \"nbody -help\".\n");
      exit(-1);
   }

   Precision = atof(gets(input));
   if (Precision == 0.0) {
      fprintf(stderr, "ERROR: The precision has no default value.\n");
      fprintf(stderr, "If you need help, type \"nbody -help\".\n");
      exit(-1);
   }
   /* Determine number of multipole expansion terms needed for specified
    * precision and flag an error if it is too precise */
   Expansion_Terms = (long) ceil(-(log(Precision) / log(BASE)));
   if (Expansion_Terms > MAX_EXPANSION_TERMS) {
      fprintf(stderr, "ERROR: %g (%ld terms) is too great a precision.\n", Precision, Expansion_Terms);
      fprintf(stderr, "If you need help, type \"nbody -help\".\n");
      exit(-1);
   }

   Number_Of_Processors = atoi(gets(input));
   if (Number_Of_Processors == 0) {
      fprintf(stderr, "ERROR: The Number_Of_Processors has no default.\n");
      fprintf(stderr, "If you need help, type \"nbody -help\".\n");
      exit(-1);
   }
   if (Number_Of_Processors < 0) {
      fprintf(stderr, "ERROR: Number of processors should be an int greater ");
      fprintf(stderr, "than 0.\n");
      fprintf(stderr, "If you need help, type \"nbody -help\".\n");
      exit(-1);
   }

   Time_Steps = atoi(gets(input));
   if (Time_Steps == 0) {
      fprintf(stderr, "ERROR: The number of time steps has no default.\n");
      fprintf(stderr, "If you need help, type \"nbody -help\".\n");
      exit(-1);
   }
   if (Time_Steps < 0) {
      fprintf(stderr, "ERROR: The number of time steps should be an int ");
      fprintf(stderr, "greater than 0.\n");
      fprintf(stderr, "If you need help, type \"nbody -help\".\n");
      exit(-1);
   }

   Timestep_Dur = atof(gets(input));
   if (Timestep_Dur == 0.0) {
      fprintf(stderr, "ERROR: The duration of a time step has no default ");
      fprintf(stderr, "value.\n If you need help, type \"nbody -help\".\n");
      exit(-1);
   }
   if (Timestep_Dur < 0) {
      fprintf(stderr, "ERROR: The duration of a time step should be a ");
      fprintf(stderr, "double greater than 0.\n");
      fprintf(stderr, "If you need help, type \"nbody -help\".\n");
      exit(-1);
   }

   Softening_Param = atof(gets(input));
   if (Softening_Param == 0.0)
      Softening_Param = MIN_REAL;
   if (Softening_Param < 0) {
      fprintf(stderr, "ERROR: The softening parameter should be a double ");
      fprintf(stderr, "greater than 0.\n");
      fprintf(stderr, "If you need help, type \"nbody -help\".\n");
      exit(-1);
   }

   gets(input);
   if ((*input == '\0') || (strcmp(input, "cost zones") == 0))
      Partition_Flag = COST_ZONES;
   else {
      if (strcmp(input, "orb") == 0)
	 Partition_Flag = ORB;
      else {
	 fprintf(stderr, "ERROR: The only partitioning schemes available ");
	 fprintf(stderr, "are \"cost zones\" \n\t or \"orb\".\n");
	 fprintf(stderr, "If you need help, type \"nbody -help\".\n");
	 exit(-1);
      }
   }
}


void
PrintTimes ()
{
   long i, j;
   time_info *timing;
   FILE *fp;
   double t_total_time = 0;
   double t_tree_time = 0;
   double t_list_time = 0;
   double t_part_time = 0;
   double t_pass_time = 0;
   double t_inter_time = 0;
   double t_bar_time = 0;
   double t_intra_time = 0;
   double t_other_time = 0;
   double total_time;
   double tree_time;
   double list_time;
   double part_time;
   double pass_time;
   double inter_time;
   double bar_time;
   double intra_time;
   double other_time;
   double overall_total = 0;
   long P;
   long init_done;

   if ((fp = fopen("times", "w")) == NULL) {
      fprintf(stderr, "Error opening output file\n");
      fflush(stderr);
      exit(-1);
   }
   fprintf(fp, "TIMING:\n");
   fprintf(fp, "%ld\t%ld\t%.2e\t%ld\n", Number_Of_Processors, Total_Particles, Precision, Time_Steps);
   for (i = 0; i < Time_Steps; i++) {
      fprintf(fp, "Time Step %ld\n", i);
      for (j = 0; j < Number_Of_Processors; j++) {
	 timing = &(Local[j].Timing[i]);
	 fprintf(fp, "Processor %ld\n", j);
	 fprintf(fp, "\tTotal Time = %lu\n", timing->total_time);
	 if (do_stats) {
	    fprintf(fp, "\tTree Construction Time = %lu\n",
		    timing->construct_time);
	    fprintf(fp, "\tList Construction Time = %lu\n", timing->list_time);
	    fprintf(fp, "\tPartition Time = %lu\n", timing->partition_time);
	    fprintf(fp, "\tTree Pass Time = %lu\n", timing->pass_time);
	    fprintf(fp, "\tInter Particle Time = %lu\n", timing->inter_time);
	    fprintf(fp, "\tBarrier Time = %lu\n", timing->barrier_time);
	    fprintf(fp, "\tIntra Particle Time = %lu\n", timing->intra_time);
	    fprintf(fp, "\tOther Time = %lu\n", timing->other_time);
	 }
	 fflush(fp);
      }
   }
   fprintf(fp, "END\n");
   fclose(fp);

   printf("                                   PROCESS STATISTICS\n");
   printf("             Track        Tree        List        Part        Pass       Inter        Bar        Intra       Other\n");
   printf(" Proc        Time         Time        Time        Time        Time       Time         Time       Time        Time\n");
   total_time = tree_time = list_time = part_time = pass_time =
   inter_time = bar_time = intra_time = other_time = 0;
   for (i = 2; i < Time_Steps; i++) {
     timing = &(Local[0].Timing[i]);
     total_time += timing->total_time;
     tree_time += timing->construct_time;
     list_time += timing->list_time;
     part_time += timing->partition_time;
     pass_time += timing->pass_time;
     inter_time += timing->inter_time;
     bar_time += timing->barrier_time;
     intra_time += timing->intra_time;
     other_time += timing->other_time;
   }
   printf(" %4d %12.0f%12.0f%12.0f%12.0f%12.0f%12.0f%12.0f%12.0f%12.0f\n",
          0,total_time,tree_time,list_time,part_time,pass_time,
          inter_time,bar_time,intra_time,other_time);
   t_total_time += total_time;
   t_tree_time += tree_time;
   t_list_time += list_time;
   t_part_time += part_time;
   t_pass_time += pass_time;
   t_inter_time += inter_time;
   t_bar_time += bar_time;
   t_intra_time += intra_time;
   t_other_time += other_time;
   if (total_time > overall_total) {
     overall_total = total_time;
   }
   for (j = 1; j < Number_Of_Processors; j++) {
     total_time = tree_time = list_time = part_time = pass_time =
     inter_time = bar_time = intra_time = other_time = 0;
     for (i = 2; i < Time_Steps; i++) {
       timing = &(Local[j].Timing[i]);
       total_time += timing->total_time;
       tree_time += timing->construct_time;
       list_time += timing->list_time;
       part_time += timing->partition_time;
       pass_time += timing->pass_time;
       inter_time += timing->inter_time;
       bar_time += timing->barrier_time;
       intra_time += timing->intra_time;
       other_time += timing->other_time;
     }
     if (do_stats) {
       printf(" %4ld %12.0f%12.0f%12.0f%12.0f%12.0f%12.0f%12.0f%12.0f%12.0f\n",
               j,total_time,tree_time,list_time,part_time,pass_time,
               inter_time,bar_time,intra_time,other_time);
     }
     t_total_time += total_time;
     t_tree_time += tree_time;
     t_list_time += list_time;
     t_part_time += part_time;
     t_pass_time += pass_time;
     t_inter_time += inter_time;
     t_bar_time += bar_time;
     t_intra_time += intra_time;
     t_other_time += other_time;
     if (total_time > overall_total) {
       overall_total = total_time;
     }
   }
   if (do_stats) {
     P = Number_Of_Processors;
     printf("  Avg %12.0f%12.0f%12.0f%12.0f%12.0f%12.0f%12.0f%12.0f%12.0f\n",
             t_total_time/P,t_tree_time/P,t_list_time/P,t_part_time/P,
	     t_pass_time/P,t_inter_time/P,t_bar_time/P,t_intra_time/P,
	     t_other_time/P);
   }
   printf("\n");
   if (Time_Steps > 2) {
     init_done = Local[0].init_done_times;
     if (do_stats) {
       for (j = 1; j < Number_Of_Processors; j++) {
         if (Local[j].init_done_times > init_done) {
           init_done = Local[j].init_done_times;
         }
       }
     }
     printf("                                   TIMING INFORMATION\n");
     printf("Start time                        : %16lu\n", starttime);
     printf("Initialization finish time        : %16lu\n", init_done);
     printf("Overall finish time               : %16lu\n", endtime);
     printf("Total time with initialization    : %16lu\n", endtime - starttime);
     printf("Total time without initialization : %16lu\n", (long) (overall_total));
     printf("\n");

     printf("Total time for steps %ld to %ld : %12.0f\n", 3L, Time_Steps, overall_total);
     printf("\n");
   }
}


void
Help ()
{
   printf("Usage: FMM <options> < inputfile\n\n");
   printf("options:\n");
   printf("  -o : Print out final particle positions.\n");
   printf("  -s : Print out individual processor timing statistics.\n");
   printf("  -h : Print out command line options\n");
   printf("\n");
   printf("Input parameter descriptions:\n");
   printf("  There are nine parameters, and parameters three through\n");
   printf("  have no default values.\n");
   printf("1) Cluster Type : Distribute particles in one cluster\n");

   printf("   (\"one cluster\") or two interacting clusters (\"two cluster\")\n");
   printf("   Default is two cluster.\n");
   printf("2) Distribution Type : Distribute particles in either a\n");
   printf("   uniform spherical distribution (\"uniform\"), or in a\n");
   printf("   Plummer model (\"plummer\").  Default is plummer.\n");
   printf("3) Number Of Particles : Integer greater than 0.\n");
   printf("4) Precision : Precision of results.  Should be a double.\n");
   printf("5) Number of Processors : Integer greater than 0.\n");
   printf("6) Number of Time Steps : Integer greater than 0.\n");
   printf("7) Time Step Duration : Double greater than 0.\n");
   printf("8) Softening Parameter : Real number greater than 0.\n");
   printf("   Defaults is DBL_MIN or FLT_MIN.\n");
   printf("9) Partitioning Scheme : \"cost zones\" or \"orb\".\n");
   printf("   Default is cost zones.\n");
   exit(0);
}

#undef MAX_LINE_SIZE
#undef BASE
