/*				
*					DEMON.C
*	Demon wyszukujący plików w systemie
*	używając podanych w argumentach fragmentów nazw
*
*	Dodatkowe opcje:
*	-v - Uwzględnij dodatkowe informacje do syslog
*	-t - Czas uśpienia demona
*
*	Wykonawcy:
*	Romanowicz Mateusz
*	Ciborowski Marek
*	Wojtach Patryk
*
*/



#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

/* Maksymalna długość ścieżki  */
#define PATH_MAX 4096
/* Maksymalna długość szukanego fragmentu  */
#define FRAGMENTBUF_MAX 50

/* Flaga ustawiana w zależności od otrzymanego sygnału  */
int receivedSignal = 0;		

/* Ilość fragmentw do wyszukania  */	
int fragmentCount = 0; 

/* Ilość działających procesów przeszukujących  */			
int runningProcesses = 0; 		

/* Tablica przechowująca indentyfikatory procesów przeszukujących  */
pid_t *children; 				


/* 
*	Działanie: 
*	Funkcja obsługująca sygnały
*
*	Argumenty:
*	sig - Liczba całkowita oznaczająca sygnał
*/
void 
SignalHandler(int sig)
{
	if (sig == SIGUSR1) 
	{
		
		receivedSignal = 1;
	}
	else if (sig == SIGUSR2) 
	{
		
		receivedSignal = 2;
	}
	else if (sig == SIGCHLD) 
	{
		pid_t processToTerminate = wait(NULL);
		int i;
		for (i = 0; i < fragmentCount; i++) 
		{
			if (children[i] == processToTerminate) 
			{
				children[i] = 0;
				runningProcesses--;
				break;
			}
		}
		receivedSignal=3;
	}
}



/*
*	Działanie:
*	Funkcja sprawdzająca wystąpienia danego łańcucha znaków w innym
*
*	Argumenty:
*	strTChck - (String To Check) łańcuch znaków w którym poszukiwany jest sFrgm
*	strFrgm - (String Fragment) Poszukiwany łańcuch znaków
*
*	Zwracana wartość:
*	1 gdy dany łańcuch znaków występuje, 0 gdy nie
*/
int 
checkForFragmentInString(char* strTChck, char* strFrgm) 
{
	int i = 0, j = 0;
	char *p1, *p2, *p3;
	p1 = strTChck;
	p2 = strFrgm;

	for (i = 0; i < strlen(strTChck); ++i) 
	{
		if (*p1 == '\0')
			return 0;
		if (*p1 == *p2) 
		{
			p3 = p1;
			for (j = 0; j < strlen(strFrgm); ++j) 
			{
				if (*p3 == *p2) 
				{
					p3++;
					p2++;
				}
				else
					break;
			}
			p2 = strFrgm;
			if (j == strlen(strFrgm))
				return 1;
		}
		p1++;
	}
	return 0;
}


/*
*	Działanie:
*	Funkcja poszukująca plików w których nazwie występuje dany łańcuch znaków
*
*	Argumenty:
*	path - ścieżka w której funkcja zaczyna przeszukiwanie
*	sTs - (String To Search) Wzorzec do odnalezienia przez funkcję
*	vOpt - (-v Option) 1 jeśli program został wywołany z -v, 0 jeśli nie
*/
void 
searchForFile(const char* path, char* sTs, int vOpt) 
{

	/* Pomocnicze zmienne do wyznaczenia ścieżki */

	char absolutePath[PATH_MAX + 1];
	char* absolutePointer = NULL;
	char newPath[PATH_MAX + 1];

	int checkFlag = 0;
	DIR* dir;
	struct dirent *entry;
	if (!(dir = opendir(path)))
		return;
	chdir(path);
	/* Przeszukuj folder */
	while ( ((entry = readdir(dir)) != NULL) && (!receivedSignal) ) 
	{

		if (strcmp(entry->d_name, ".") == 0
			|| strcmp(entry->d_name, "..") == 0)
			continue;
		/* Przygotowanie nowej ścieżki */
		snprintf(newPath, sizeof(newPath), "%s/%s", path, entry->d_name);

		/* Porównanie nazwy pliku z fragmentem */
		checkFlag = checkForFragmentInString(entry->d_name, sTs);

		/* Dodatkowe informacje dla -v */
		if (vOpt) 
		{
			absolutePointer = realpath(newPath, absolutePath);
			switch (checkFlag) 
			{
			case 1:
				syslog(LOG_DAEMON | LOG_INFO,
					"Comparing file name at: [%s] ...String matches!",
					absolutePointer);
				break;
			case 0:
				syslog(LOG_DAEMON | LOG_INFO,
					"Comparing file name at: [%s] ...No match.",
					absolutePointer);
				break;
			}

		}

		/* Wysyłanie informacji do syslogu w przypadku znalezionego pliku */
		if (checkFlag) 
		{

			absolutePointer = realpath(newPath, absolutePath);
			time_t t = time(NULL);
			struct tm *date = localtime(&t);

			syslog(LOG_DAEMON | LOG_NOTICE, 
				"Found file! Date: %d-%d-%d %d:%d:%d "
				"Path: %s Searched string: %s\n", 
				date->tm_year + 1900, date->tm_mon + 1,
				date->tm_mday, date->tm_hour,
				date->tm_min, date->tm_sec,
				absolutePointer, sTs);

		}

		/* Dla każdego folderu wywołaj kolejną rekurencję */
		if (entry->d_type == DT_DIR)
			searchForFile(newPath, sTs, vOpt);
	}
	if (receivedSignal == 1) 
	{
		char restartDirectory[PATH_MAX];
		getcwd(restartDirectory,PATH_MAX);
		if (strcmp(restartDirectory, "/") == 0)
		{
			searchForFile(".", sTs, vOpt);
		}
		else 
			return;
		
	}
	if (receivedSignal == 2)
	{
		return;
	}
	chdir("..");
	closedir(dir);	
}

int 
main(int argc, char** argv)
{
	int i, secondOptRead=0;
	
	/* Tablica zawierająca fragmenty nazwy plików */
	char** fragments;
	/* Czas snu demona */
	int sleepTime = 5;
	/* Zmienna sprawdzająca czy wywołano demona z opcją -v */
	int vOpt = 0;

	/* Zliczenie ilości wzorców(fragmentów nazw) */
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "-t") == 0)
			break;
		fragmentCount++;
	}
	


	if (fragmentCount == 0)
	{
		printf("Error: Wrong command syntax\n"
			"Correct syntax: ./demon <strings> <-v> <-t sleepTime>"
			"strings\t Search files with <strings> in name\n"
			"-v\t Log additional data\n"
			"-t\t Set demon sleep time (seconds)\n");
		exit(-1);
	}

	/* Zapisanie podanych fragmentów do tablicy */
	else
	{
		fragments = (char**)malloc(fragmentCount * sizeof(char*));
		for (i = 0; i < fragmentCount; ++i)
			fragments[i] = (char*)malloc(FRAGMENTBUF_MAX * sizeof(char));

		for (i = 0; i < fragmentCount; i++)
			strcpy(fragments[i], argv[i + 1]);

		/* Sprawdzenie wyst�powania dodatkowych opcji */

		/* Tylko dodatkowe info */
		
		if (argc == fragmentCount + 2 
			&& strcmp(argv[fragmentCount + 1], "-v") == 0)
			vOpt = 1;

		/* Tylko ustawienie czasu snu */
		else
		{
			if (argc == fragmentCount + 3 
				&& strcmp(argv[fragmentCount + 1], "-t") == 0)
			{
				if((secondOptRead = atoi(argv[fragmentCount + 2])) != 0)
				{
					sleepTime = secondOptRead;
				}
				else
					secondOptRead = -1;
			}

			/* Obie dodatkowe opcje */
			else 
			{
				if (argc == fragmentCount + 4 
					&& strcmp(argv[fragmentCount + 1], "-v") == 0 
					&& strcmp(argv[fragmentCount + 2], "-t") == 0)
				{
					vOpt = 1;
					if ((secondOptRead = atoi(argv[fragmentCount + 3])) != 0)
					{
						sleepTime = secondOptRead;
					}
					else
						secondOptRead = -1;
				}
			}
		}
	}
	if (secondOptRead < 0)
	{
		printf("Error: Wrong command syntax\n"
			"Correct syntax: ./demon <strings> <-v> <-t sleepTime>"
			"strings\t Search files with <strings> in name\n"
			"-v\t Log additional data\n"
			"-t\t Set demon sleep time (seconds)\n");
		exit(-1);
	}
	

	/* Tworzenie procesu nadzorczego */
	pid_t pid;

	/* Nowy proces */
	pid = fork();
	if (pid == -1)
		return -1;
	else if (pid != 0)
		exit(EXIT_SUCCESS);	/* Zakończ proces rodzica */

	/* Nowa sesja i grupa procesów */
	if (setsid() == -1)
		return -1;

	/* Ustaw katalog na katalog główny */
	if (chdir("/") == -1)
		return -1;

	/* Zamknięcie otwartych plików */
	for (i = 0; i < 4; ++i)
		close(i);

	/* Przeadresowanie deskryptorów plików 0, 1 i 2 na /dev/null */
	open("/dev/null", O_RDWR);	/* stdin */
	dup(0);	/* stdout */
	dup(0);	/* stderror */

	/* Otwarcie logu systemowego */
	openlog("SearchingDemon", LOG_PID, LOG_DAEMON);

	children = (pid_t*)malloc(fragmentCount * sizeof(pid_t));
	pid_t parent = getpid();
	

	/* Obsługa sygnałów */
	struct sigaction newAction, oldAction;
	newAction.sa_handler = SignalHandler;
	sigemptyset(&newAction.sa_mask);
	newAction.sa_flags = 0;
	if (sigaction(SIGUSR1, &newAction, &oldAction)<0)
	{
		syslog(LOG_ERR, "Cannot handle SIGUSR1\n");
		exit(-1);
	}
	if (sigaction(SIGUSR2, &newAction, &oldAction)<0)
	{
		syslog(LOG_ERR, "Cannot handle SIGUSR2\n");
		exit(-1);
	}
	if (sigaction(SIGCHLD, &newAction, &oldAction)<0)
	{
		syslog(LOG_ERR, "Cannot handle SIGCHLD\n");
		exit(-1);
	}
	syslog(LOG_INFO,"Demon %d starts working",parent);

	
	int processIndex;
	while (1) 
	{
		/* Działanie procesu nadzorczego */
		if (getpid() == parent) 
		{
			if(vOpt)
				syslog(LOG_INFO, "Demon goes to sleep");
			sleep(sleepTime);
			switch (receivedSignal)
			{
				case 0:
					if (vOpt)
						syslog(LOG_INFO, "Demon awakens");

					/* Zamknięcie istniejących procesów, jeśli istnieją */
					if (runningProcesses)
					{
						for (i = 0; i < fragmentCount; i++)
						{
							if (!runningProcesses)
								break;
							if (children[i])
								kill(children[i],SIGKILL);
						}
					}
					/* 
					* Utworzenie procesów przeszukujących
					* o liczbie równej ilości fragmentów nazw
					* oraz zapisanie ich identyfikatorów
					*/
					processIndex=0;
					for (i = 0; i < fragmentCount; i++)
					{
						if (getpid() == parent)
						{
							pid_t newChildren = fork();
								if (pid == -1)
									exit(-1);
								else if (pid != 0)
								{
									children[i] = pid;
									runningProcesses++;
									processIndex++;
								}
								else if (pid == 0)
									break;
						}
					}
					break;
				case 1:
					if(vOpt)
						syslog(LOG_INFO, "Catched SIGUSR1\n");

					/* 
					* Jeśli istnieją działające procesy, 
					* wysyłany jest do nich sygnał
					* W przypadku zako�czonych tworzone są nowe
					*/
					if (runningProcesses)
					{
						for (i = 0; i < fragmentCount; i++)
						{
							if (children[i])
								kill(children[i], SIGUSR1);
							else
							{
								processIndex=i;
								pid_t newChildren = fork();
									if (pid == -1)
										exit(-1);
									else if (pid != 0)
										children[i] = pid;
									else if (pid == 0)
										break;
							}
						}
					}
					/*
					* Jeśli nie istnieją pracujące procesy
					* tworzone są nowe
					*/
					
					else
					{
						processIndex=0;
						for (i = 0; i < fragmentCount; i++)
						{
							if (getpid() == parent)
							{
								pid_t newChildren = fork();
									if (pid == -1)
										exit(-1);
									else if (pid != 0)
										children[i] = pid;
									else if (pid == 0)
										break;
							}
						}
					}
					receivedSignal = 0;
					break;

				case 2:
					if (vOpt)
						syslog(LOG_INFO, "Catched SIGUSR2\n");


					if (runningProcesses)
					{
						for (i = 0; i < fragmentCount; i++)
						{
							if (!runningProcesses)
								break;
							if (children[i])
								kill(children[i], SIGUSR2);
						}
					}
					receivedSignal = 0;
					break;
				case 3:
				if (vOpt)
						syslog(LOG_INFO,
							"Demon awakens due to SIGCHLD\n");
					receivedSignal=0;
					break;
			}
		}
		else 
			break;
		
	}

	/* 
	* Wywołanie operacji wyszukiwania
	* dla każdego z procesów potomnych
	*/
	for (i = 0; i < fragmentCount; i++)
	{
		if (processIndex == i)
		{
			searchForFile("/", fragments[i], vOpt);
			break;
		}
	}
	
	exit(0);
}
