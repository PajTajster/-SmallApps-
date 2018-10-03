/*
*			No_Starvation.c
*	Readers-Writers simulating program.
*
*	First 2 args are for respectively writers and readers
*	threads amount.
*
*	Resolution with no starvation.
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

/* Conditional Variable deciding whether writers can use resource or not */
pthread_cond_t canWriterIn = PTHREAD_COND_INITIALIZER;
/* Conditional Variable deciding whether readerss can use resource or not */
pthread_cond_t canReadersIn = PTHREAD_COND_INITIALIZER;

/* Semaphore blocking UpdateStatus() function */
sem_t semStatus;

/* canWriterIn cond variable mutex */
pthread_mutex_t mutexCondWriter = PTHREAD_MUTEX_INITIALIZER;
/* canReadersIn cond variable mutex */
pthread_mutex_t mutexCondReader = PTHREAD_MUTEX_INITIALIZER;

/* Anti-Race mutex for Writers */
pthread_mutex_t mutexWriter = PTHREAD_MUTEX_INITIALIZER;
/* Anti-Race mutex for Readers */
pthread_mutex_t mutexReader = PTHREAD_MUTEX_INITIALIZER;

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
	printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n",
		queuedReaders, queuedWriters,
		criticalReaders, criticalWriters);
}

/*
*	Readers thread function, readers enters and checks whether somebody
*	uses the resource, if not, he gets in and works, otherwise he waits
*	till the conditional variable lets him in.
*
*	Last leaving reader lets in the first waiting writer
*/
void*
Reader(void* arg)
{
	do
	{
		/* ENTRY - BEGIN */
		
		/* Avoid race to update status */
		if(pthread_mutex_lock(&mutexCondReader))
		{
			printf("Error while locking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}		
		
		/* Update status */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		++queuedReaders;
		UpdateStatus();
		
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		/*
		*	If first to enter, then go ahead and use resource
		*	if not, then wait.
		*/
		if(!(criticalReaders == 0 && criticalWriters == 0) 
			|| !(queuedWriters == 0))
		{
			if(pthread_cond_wait(&canReadersIn, &mutexCondReader))
			{
				printf("Error while blocking on condition variable, exiting\n");
				exit(EXIT_FAILURE);
			}
		}
		
		/* Unlock ENTRY for others */
		if(pthread_mutex_unlock(&mutexCondReader))
		{
			printf("Error while unlocking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}		
		/* ENTRY - END */
		
		
		/* WORKING - BEGIN */
		
		/* Prevent races while updating status */
		if(pthread_mutex_lock(&mutexReader))
		{
			printf("Error while locking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}		
		
		/* Guess what, update status! */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		--queuedReaders;
		++criticalReaders;
		UpdateStatus();
		
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		/* Unlock the mutex */
		if(pthread_mutex_unlock(&mutexReader))
		{
			printf("Error while unlocking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}		
		
		/* Work work */
		sleep(2);
		/* WORKING - END */
		
		/* EXIT - BEGIN */
		
		/* Avoid races by locking the mutex to update status */
		if(pthread_mutex_lock(&mutexReader))
		{
			printf("Error while locking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}		
		
		/* Status, update it */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		--criticalReaders;
		UpdateStatus();
		
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		/*
		*	If last leaving, let in 1 writer 
		*	if no writers are present, let in all currently waiting readers
		*/
		if(criticalReaders == 0)
		{	
			if(queuedWriters)
			{
				if(pthread_cond_signal(&canWriterIn))
				{
					printf("Error while unblocking, exiting\n");
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				if(pthread_cond_broadcast(&canReadersIn))
				{
					printf("Error while unblocking, exiting\n");
					exit(EXIT_FAILURE);
				}
			}
		}
		
		/* Unlock EXIT for others */
		if(pthread_mutex_unlock(&mutexReader))
		{
			printf("Error while unlocking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}
		/* EXIT - END */
		
	}
	while(1);
}

/*
*	Writers thread function, the writers comes, looks around
*	if nobody is using resource then he gets in and takes it
*	otherwise he waits for the signal
*
*	Leaving writers lets in all currently waiting readers
*	if there's none, he lets in a writer
*/
void*
Writer(void* arg)
{
	do
	{
		/* ENTRY - BEGIN */
		
		/* Avoid races by locking mutex */
		if(pthread_mutex_lock(&mutexCondWriter))
		{
			printf("Error while locking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}
		
		/* Update status */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		++queuedWriters;
		UpdateStatus();
		
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		/* If first to enter, take the resource, if not, wait */
		if( !(criticalReaders == 0 && criticalWriters == 0)
			|| !(queuedReaders == 0) )
		{
			if(pthread_cond_wait(&canWriterIn, &mutexCondWriter))
			{
				printf("Error while blocking on condition variable, exiting\n");
				exit(EXIT_FAILURE);
			}
		
		}
		
		/* Unlock anti-race mutex */
		if(pthread_mutex_unlock(&mutexCondWriter))
		{
			printf("Error while unlocking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}
		/* ENTRY - END */
		
		/* WORKING - BEGIN */
		
		/* Avoid races by locking mutex */
		if(pthread_mutex_lock(&mutexWriter))
		{
			printf("Error while locking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}
		
		/* Update status */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		--queuedWriters;
		++criticalWriters;
		UpdateStatus();
		
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		/* Unlock mutex */
		if(pthread_mutex_unlock(&mutexWriter))
		{
			printf("Error while unlocking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}
		
		/* Work */
		sleep(2);
		/* WORKING - END */

		/* EXIT - BEGIN */		
		
		/* Lock the mutex to avoid races */
		if(pthread_mutex_lock(&mutexWriter))
		{
			printf("Error while locking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}
		
		/* Update status */
		if(sem_wait(&semStatus) == -1)
		{
			printf("Error while locking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		--criticalWriters;
		UpdateStatus();
		
		if(sem_post(&semStatus) == -1)
		{
			printf("Error while unlocking semaphore, exiting [%s]\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		
		/* Upon leaving, let in waiting readers, if none, let in a writer */
		if(queuedReaders >= 1)
		{
			if(pthread_cond_broadcast(&canReadersIn))
			{
				printf("Error while unblocking, exiting\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			if(pthread_cond_signal(&canWriterIn))
			{
				printf("Error while unblocking, exiting\n");
				exit(EXIT_FAILURE);
			}
		}
		
		/* Unlock anti-race mutex */
		if(pthread_mutex_unlock(&mutexWriter))
		{
			printf("Error while unlocking mutex, exiting\n");
			exit(EXIT_FAILURE);
		}
		/* EXIT - END */		
		
	}
	while(1);
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
	
	/* Semaphore init */
	sem_init(&semStatus, 0, 1);
	
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
	
	if(pthread_cond_destroy(&canWriterIn))
	{
		printf("Error while destroying condition variable, exiting\n");
		exit(EXIT_FAILURE);
	}
	if(pthread_cond_destroy(&canReadersIn))
	{
		printf("Error while destroying condition variable, exiting\n");
		exit(EXIT_FAILURE);
	}
	
	if(pthread_mutex_destroy(&mutexCondWriter))
	{
		printf("Error while destroying mutex, exiting\n");
		exit(EXIT_FAILURE);
	}
	
	if(pthread_mutex_destroy(&mutexCondReader))
	{
		printf("Error while destroying mutex, exiting\n");
		exit(EXIT_FAILURE);
	}
	
	if(pthread_mutex_destroy(&mutexWriter))
	{
		printf("Error while destroying mutex, exiting\n");
		exit(EXIT_FAILURE);
	}
	
	if(pthread_mutex_destroy(&mutexReader))
	{
		printf("Error while destroying mutex, exiting\n");
		exit(EXIT_FAILURE);
	}
	
	/* More cleaning */
	free(writers);
	free(readers);

	
	exit(EXIT_SUCCESS);
}
