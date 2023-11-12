#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/mman.h>

// Author: Quan Nguyen
// Sources: Activity 3, StackOverflow

// global variables
sem_t* semaphore;
pid_t otherPid;
sigset_t sigSet;

void signalHandler1(int signum)
{
	printf("Caught signal: %d\n", signum);
	printf("     Exit first process.\n");
	sem_post(semaphore);
	_exit(0);
}

void signalHandler2(int signum)
{
	printf("I am alive!\n");
}

// logic to run to simulate a parent process
void childProcess()
{
	// setup some signal handlers
	signal(SIGUSR1, signalHandler1);
	signal(SIGUSR2, signalHandler2);
	
	// child process
	int value;
	sem_getvalue(semaphore, &value);
	printf("    First process semaphore count is %d.\n", value);
	printf("    First process is grabbing semaphore.\n");
	sem_wait(semaphore);
	sem_getvalue(semaphore, &value);
	printf("    First process semaphore count is %d.\n", value);
	// START CRITICAL REGION
	printf("    Starting very long first process.......\n");
	for(int x=0;x<60;x++)
	{
		printf(".\n");
		sleep(1);
	}
	// END CRITICAL REGION
	sem_post(semaphore);
	
	// exit the child process
	printf("     Exit first process.\n");
	_exit(0);
}

void *checkHungChild( void *a)
{
	// simulate a timer of 10 sec
	int* status = a;
	printf("Checking for hung first process......\n");
	sleep(10);
	if(sem_trywait(semaphore) != 0)
	{
		printf("First process appears to be hung.....\n");
		*status = 1;
	}
	else
	{
		printf("First procoess appears to running fine...\n");
		*status = 0;
	}
	return NULL;
}

// logic to run to isimulate a parent process
void parentProcess()
{
	// parent process
	sleep(10);
	if(getpgid(otherPid) >= 0)
	{
		printf("First process is running....\n");
	}
	int value;
	sem_getvalue(semaphore, &value);
	printf("In the second process with the semaphore count of %d.\n", value);
	
	// try to get semaphore 
	if(sem_trywait(semaphore) != 0)
	{
		// start timer
		pthread_t tid1;
		int status = 0;
		printf("Detected First is hung or running too long....\n");
		if(pthread_create(&tid1, NULL, checkHungChild, &status))
		{
			printf("ERROR creating timer thread.\n");
			_exit(1);
		}
		if(pthread_join(tid1, NULL))
		{
			printf("\n ERROR joining timer thread.\n");
			exit(1);
		}
		
		// see if we need to kill the child process
		if(status == 1)
		{
			// kill child process
			printf("Going to kill First process with ID of %d\n", otherPid);
			kill(otherPid, SIGUSR1);
			kill(otherPid, SIGTERM);
			printf("Killed First process\n");
			
			// prove that the child process is killed
			printf("Now proving First process is killed (you should see no dots and no response from SIGUSR2 sginal)\n");
			sleep(5);
			kill(otherPid, SIGUSR2);
			sleep(3);
			printf("Done proving First process is killed\n");
			
			// try to get semaphore
			sem_getvalue(semaphore, &value);
			printf("In the second process with the semaphroe count of %d.\n", value);
			if(sem_trywait(semaphore) != 0)
			{
				if(value == 0)
				sem_post(semaphore);
				printf("Cleaned up and finally got the semaphore.\n");
				sem_getvalue(semaphore, &value);
				printf("in the second process with the semaphore of %d.\n", value);
			}
			else
			{
				printf("Finally got the semaphore.\n");
			}
		}
	}
	
	// exit the parent process
	printf("Exit second process.\n");
	_exit(0);
}

// main entry app
int main(int argc, char* argv[])
{
	pid_t pid;
	
	// create shared semaphore
	semaphore = (sem_t*)mmap(0,sizeof(sem_t), PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if(sem_init(semaphore, 1, 1) != 0)
	{
		printf("failed to create semaphore.\n");
		exit(EXIT_FAILURE);
	}
	
	// use fork()
	pid = fork();
	if(pid == -1)
	{
		// error
		printf("Can't fork, error.\n");
		exit(EXIT_FAILURE);
	}
	// OK
	if(pid == 0)
	{
		// run child process logic
		printf(     "Started First process, ID: %d....\n", getpid());
		otherPid = getppid();
		childProcess();
	}
	else
	{
		// run parent process logic
		printf("Started second process, ID: %d......\n", getpid());
		otherPid = pid;
		parentProcess();
	}
	
	// cleanup 
	sem_destroy(semaphore);
	
	// return OK
	return 0;
}