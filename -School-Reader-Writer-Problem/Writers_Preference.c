/*
*			Writers_Preference.c
*	A program to simulate Reader-Writer problem.
*
*	Solution favoring writers (starving readers)
*
*	Creators:
*	Romanowicz Mateusz
*	Ciborowski Marek
*	Wojtach Patryk
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* Max thread amount */
#define MAX_WRITER_AMOUNT 2
#define MAX_READER_AMOUNT 14

/* Thread arrays */
pthread_t writers[MAX_WRITER_AMOUNT];
pthread_t readers[MAX_READER_AMOUNT];


/* Resource blocking semaphore */
sem_t semResource;
/* Semaphore used for blocking users off resource */
sem_t semTryRead;
/* Race condition avoiding semaphore (for readers) */
sem_t mutexReader;
/* Race condition avoiding semaphore (for writers) */
sem_t mutexWriter;
/* UpdateStatus() function access blocking semaphore */
sem_t semStatus;


/* Writers waiting to access resource */
volatile int queuedWriters = 0;
/* Readers waiting to access resource */
volatile int queuedReaders = 0;
/* Readers currently working */
volatile int criticalReaders = 0;
/* Writers currently working */
volatile int criticalWriters = 0;

/*
*	Operation:
*	Function prints that prints current state to the console in such manner:
*	
*	ReaderQ: a WriterQ: b [in: R:c W:d] 
*
*	a - queued readers
*	b - queued writers
*	c - working readers
*	d - working writers
*
*/
void
UpdateStatus()
{
	int tmp1 = queuedReaders;
	int tmp2 = queuedWriters;
	if(queuedReaders)
		tmp1 -=criticalReaders;
	if(queuedWriters)
		tmp2 -=criticalWriters;
	printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n",
		tmp1, tmp2,
		criticalReaders, criticalWriters);
}

/*
*	Operation:
*	Reader thread function, every reader asks separately for resource access.
*/
void*
Reader(void* arg)
{
	do{
		/* ENTRY - BEGIN */	
		
		/* Avoid race due to status update */
		sem_wait(&mutexReader);
		
		/* Update status */
		sem_wait(&semStatus);
		queuedReaders++;	
		UpdateStatus();
		sem_post(&semStatus);
		
		sem_post(&mutexReader);
	
		/* Reader tries to enter */
		sem_wait(&semTryRead);
		/* Avoid race due to if check */
		sem_wait(&mutexReader);

		/* If first reader, then lock the resource */
		if(criticalReaders == 0) 
			sem_wait(&semResource);	
		/* Unlock race avoidance block */
		sem_post(&mutexReader);
	
		/* Reader enters WORKING state */
		sem_post(&semTryRead);
		/* ENTRY - END */
	
	
		/* WORKING - BEGIN */
	
		/* Avoid race due to status update */
		sem_wait(&mutexReader);
	
		/* Update status */
		sem_wait(&semStatus);
		criticalReaders++;
		UpdateStatus();
		sem_post(&semStatus);
	
		sem_post(&mutexReader);
	
		/* Work */
		sleep(3);
		/* WORKING - END */
	
	
		/* EXIT - BEGIN */
		sem_wait(&mutexReader);
	
		/* Update status */
		sem_wait(&semStatus);
		criticalReaders--;
		queuedReaders--;
		UpdateStatus();
		sem_post(&semStatus);
		
		/* If last reader leaving working state, unlock resource */
		if(criticalReaders == 0)
			sem_post(&semResource);
	
		sem_post(&mutexReader);
		/* EXIT - END */
	} while(1);
}

/*
*	Operation:
*	Writer thread function. First writer to enter blocks all the readers
*	who are waiting out of resource. 
*/
void*
Writer(void* arg)
{
	do {
		/* ENTRY - BEGIN */
		/* Avoid race due to status update */
		sem_wait(&mutexWriter);
	
		/* Update status */
		sem_wait(&semStatus);
		queuedWriters++;
		UpdateStatus();
		sem_post(&semStatus);
	
		/* If first writer, block off waiting readers */
		if(queuedWriters == 1)
			sem_wait(&semTryRead);
	
		/* Release race avoiding block */
		sem_post(&mutexWriter);	
		/* ENTRY - END */
	
	
		/* WORKING - BEGIN */
		/* Take resource for yourself */
		sem_wait(&semResource);
	
		/* Avoid race due to status update */
		sem_wait(&mutexWriter);
	
		/* Update status */
		sem_wait(&semStatus);
		criticalWriters++;
		UpdateStatus();
		sem_post(&semStatus);
	
		sem_post(&mutexWriter);
	
		/* Work */
		sleep(4);
	
		/* Give the resource to all the hungry writers */
		sem_post(&semResource);
		/* WORKING - END */
	
	
		/* EXIT - BEGIN */
		/* Avoid race due to status update */
		sem_wait(&mutexWriter);
	
		/* Update status */
		sem_wait(&semStatus);
		criticalWriters--;
		queuedWriters--;
		UpdateStatus();
		sem_post(&semStatus);
	
		/* If last writer working, let readers use the resource */
		if(queuedWriters == 0)
			sem_post(&semTryRead);
	
		sem_post(&mutexWriter);
		/* EXIT - END */
	} while(1);
}

int
main()
{
	int i = 0;
	/* Semaphores initialize */
	sem_init(&semResource, 0, 1);
	sem_init(&semTryRead, 0, 1);
	sem_init(&mutexWriter, 0, 1);
	sem_init(&mutexReader, 0, 1);
	sem_init(&semStatus, 0, 1);
	
	/* Creating threads */
	for(i = 0; i < MAX_WRITER_AMOUNT; ++i)
		pthread_create(&writers[i], NULL, &Writer, NULL);
	for(i = 0; i < MAX_READER_AMOUNT; ++i)
		pthread_create(&readers[i], NULL, &Reader, NULL);
	
	/* Joining threads */
	for(i = 0; i < MAX_WRITER_AMOUNT; ++i)
		pthread_join(writers[i], NULL);
	for(i = 0; i < MAX_READER_AMOUNT; ++i)
		pthread_join(readers[i], NULL);
	
	return 0;
}
