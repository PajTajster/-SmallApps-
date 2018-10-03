/*
*			Readers_Preference.C
*	
*	Program simulating Reader-Writer problem with readers-preference.
*
*	First 2 args are for respectively writers and readers
*	threads amount.
*	
*
*	Authors:
*	Romanowicz Mateusz
*	Ciborowski Marek
*	Wojtach Patryk
*/



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/* Default thread amount */
#define DEFAULT_WRITER_AMOUNT 2
#define DEFAULT_READER_AMOUNT 14

/* Resource blocking semaphore*/
sem_t semResource;
/* Anti-race semaphore [Readers] */
sem_t mutexReader;
/* Anti-race semaphore [Writers] */
sem_t mutexWriter;
/* Semaphore blocking UpdateStatus() function */
sem_t semStatus;


/* Writers waiting to enter resource */
volatile int queuedWriters = 0;
/* Readers waiting to enter resource */
volatile int queuedReaders = 0;
/* Working readers */
volatile int criticalReaders = 0;
/* Working writers */
volatile int criticalWriters = 0;

/*
*	A function to print out current threads status in such manner:
*	
*	ReaderQ: a WriterQ: b [in: R:c W:d] 
*
*	a - queued readers
*	b - queued writers
*	c - working readers
*	d - working writers
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
*	Reader thread function, first reader to come tries to block the
*	resource for writers and opens it to other readers. 
*	Last reader to use the resource unlocks it for the writers 
*	the moment he finishes his operation.
*/
void*
Reader()
{
	do
	{ 
		/* ENTRY - BEGIN */
		
		/* Block the ENTRY for others to avoid race */
		if(sem_wait(&mutexReader) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* Update status */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		queuedReaders++;	
		UpdateStatus();
		
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* If first reader to enter, block the resource for writers */
		if(queuedReaders == 1)
		{
			if(sem_wait(&semResource) == -1)
			{
				printf("Error while locking semaphore, exiting [%s]\n",
					strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		/* Unlock ENTRY to another reader */
		if(sem_post(&mutexReader) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		/* ENTRY - END */
	
		/* WORKING - BEGIN */
		
		/* Avoid races by locking the WORKING while updating status */
		if(sem_wait(&mutexReader) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* update status */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		criticalReaders++;
		UpdateStatus();
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* Unlock WORKING */
		if(sem_post(&mutexReader) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* WORK */
		sleep(2);
		/* WORKING - END */
	
	
		/* EXIT - BEGIN */
		/* Avoid races by blocking EXIT while updating status */ 
		if(sem_wait(&mutexReader) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
			
		/* Update that sexy status */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		criticalReaders--;
		queuedReaders--;
		UpdateStatus();
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* If last to leave, unlock resource for writers */
		if(queuedReaders == 0)
		{
			if(sem_post(&semResource) == -1)
			{
				printf("Error while unlocking semaphore, exiting [%s]\n",
					strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	
		/* Unlock EXIT to use for others */
		if(sem_post(&mutexReader) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		/* EXIT - END */		
	}
	while(1);
}

/*
*	Writers thread function, writer comes and tries to get the resource
*	he gets the access only when there's no readers waiting for it or
*	no writer is currently working on it.
*/
void*
Writer()
{
	do {
		/* ENTRY - BEGIN */
		
		/* Avoid race while updating status */
		if(sem_wait(&mutexWriter) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* Update status */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		queuedWriters++;	
		UpdateStatus();
		
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* Unlock ENTRY to other writers */
		if(sem_post(&mutexWriter) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* Wait for the resource to be available */
		if(sem_wait(&semResource) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		/* ENTRY - END */
	
		/* WORKING - BEGIN */
		/* Avoid races while updating status */
		if(sem_wait(&mutexWriter) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		/* Update status */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		criticalWriters++;	
		UpdateStatus();
		
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* Unlock anti-race semaphore */
		if(sem_post(&mutexWriter) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* WORK */
		sleep(2);
		/* WORKING - END */
	
	
		/* EXIT - BEGIN */
		/* Avoid races while updating status */
		if(sem_wait(&mutexWriter) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* Update status */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		criticalWriters--;
		queuedWriters--;
		UpdateStatus();
		
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		/* Unlock resource for others to use */
		if(sem_post(&semResource) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		/* Unlock EXIT for others*/
		if(sem_post(&mutexWriter) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		/* EXIT - END */
	} while(1);
}

int
main(int argc, char **argv)
{
	int writersThreads = 0, readersThreads = 0;
	
	/* If no args, then default values */
	if(argc != 3)
	{
		printf("Using default thread number:\nWriters: %d, Readers: %d\n",
				DEFAULT_WRITER_AMOUNT, DEFAULT_READER_AMOUNT);
				
		writersThreads = DEFAULT_WRITER_AMOUNT;
		readersThreads = DEFAULT_READER_AMOUNT;
	}
	/* If args then read them */
	else
	{
		writersThreads = atoi(argv[1]);
		readersThreads = atoi(argv[2]);
		printf("Using user-defined thread number:\n"
				"Writers:%d, Readers%d\n",
				writersThreads, readersThreads);
	}
	
	pthread_t *writers = malloc(sizeof(pthread_t) * writersThreads);
	pthread_t *readers = malloc(sizeof(pthread_t) * readersThreads);
		
	if(writers == NULL || readers == NULL)
	{
		printf("Error while allocating memory [%s]\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	int i = 0;
	/* Semaphores init */
	if(sem_init(&semResource, 0, 1) == -1)
	{
		printf("Error while initializing semaphore, exiting [%s]\n",
			strerror(errno));
	}
	
	if(sem_init(&mutexReader, 0, 1) == -1)
	{
		printf("Error while initializing semaphore, exiting [%s]\n",
			strerror(errno));
	}
	
	if(sem_init(&mutexWriter, 0, 1) == -1)
	{
		printf("Error while initializing semaphore, exiting [%s]\n",
			strerror(errno));
	}
	
	if(sem_init(&semStatus, 0, 1) == -1)
	{
		printf("Error while initializing semaphore, exiting [%s]\n",
			strerror(errno));
	}
	
	/* Creating threads */
	for(i = 0; i < writersThreads; ++i)
	{
		if(pthread_create(&writers[i], NULL, &Writer, NULL))
		{
			printf("Error while creating a thread, exiting\n");
			exit(EXIT_FAILURE);
		}
	}
	for(i = 0; i < readersThreads; ++i)
	{
		if(pthread_create(&readers[i], NULL, &Reader, NULL))
		{
			printf("Error while creating a thread, exiting\n");
			exit(EXIT_FAILURE);
		}
	}
	
	/* Joining threads */
	for(i = 0; i < writersThreads; ++i)
	{
		if(pthread_join(writers[i], NULL))
		{
			printf("Error while joining a thread, exiting\n");
			exit(EXIT_FAILURE);
		}
	}
	for(i = 0; i < readersThreads; ++i)
	{
		if(pthread_join(readers[i], NULL))
		{
			printf("Error while joining a thread, exiting\n");
			exit(EXIT_FAILURE);
		}
	}
	
	
	/* Cleaning */
	free(writers);
	free(readers);
	
	exit(EXIT_SUCCESS);
}
