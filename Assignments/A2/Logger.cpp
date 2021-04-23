/*
Sahil Patel (159-065-176)
Abdulbasid Guled (156-024-184)
Andre Jenah (134-901-180)
*/

// Header Files
	// basic input and output services 
#include <iostream>

	// performing input and output
#include <stdio.h> 

	// defines one variable type, one macro, and various functions for manipulating arrays of characters
#include <string.h> 

	// performing general functions
#include <stdlib.h> 

	// makes a socket available 
#include <sys/socket.h> 

	// defines miscellaneous symbolic constants and types, and declares miscellaneous functions 
#include <unistd.h> 

	// includes a number of definitions for different types
#include <sys/types.h> 

	// used to protect shared data from being simultaneously accessed by multiple threads
#include <mutex>

	// makes available the type in_port_t and the type in_addr_t  
#include <arpa/inet.h> 

	// includes a number of definitions for different types
#include <netinet/in.h> 

	// Header that declares the thread class and the this_thread namespace 
#include <thread>

	// defines a number of sysmbols that relate to threads 
#include <pthread.h>

// to access the .h file
#include "Logger.h"

// to avoid the use of using std:: each time
using namespace std;

// All global variables 
int fd;
const int BUF_LEN = 256, port = 1152;
char buf[BUF_LEN];
bool isWorking = true;
struct sockaddr_in server_address;
string ip_address = "127.0.0.1";

LOG_LEVEL level_log = ERROR;
socklen_t addressLength;
mutex mutexes;

void receiveThread(int t_fd);

/*
create a non-blocking socket for UDP communications (AF_INET, SOCK_DGRAM).
Set the address and port of the server.
Create a mutex to protect any shared resources.
Start the receive thread and pass the file descriptor to it.
*/
void InitializeLog()
{
	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (fd < 0)
	{
		perror("ENCOUNTERED ERROR: Socket was unable to be created");
		exit(EXIT_FAILURE);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
	server_address.sin_port = htons(port);
	addressLength = sizeof(server_address);
	thread threadReceiver(receiveThread, fd);
	threadReceiver.detach();
}

// set the filter log level and store in a variable global
void SetLogLevel(LOG_LEVEL level)
{
	level_log = level;
}

/*
compare the severity of the log to the filter log severity. The log will be thrown away if its severity is lower than the filter log severity.
create a timestamp to be added to the log message. Code for creating the log message will look something like:
apply mutexing to any shared resources used within the Log() function.
The message will be sent to the server via UDP sendto().
*/
void Log(LOG_LEVEL level, const char* prog, const char* func, int line, const char* message)
{
	if (level > level_log)
	{
		time_t now = time(0);
		char* dt = ctime(&now);
		memset(buf, 0, BUF_LEN);
		char levelStr[][16] = { "DEBUG", "WARNING", "ERROR", "CRITICAL" };
		int len = sprintf(buf, "%s %s %s:%s:%d %s\n", dt, levelStr[level], prog, func, line, message) + 1;
		buf[len - 1] = '\0';

		sendto(fd, buf, len, 0, (struct sockaddr*)&server_address, addressLength);
	}
}

// will stop the receive thread via an isWorking flag and close the file descriptor.
void ExitLog()
{
	isWorking = false;
	close(fd);
}

/*
accept the file descriptor as an argument.
run in an endless loop via an is_running flag.
apply mutexing to any shared resources used within the recvfrom() function.
ensure the recvfrom() function is non-blocking with a sleep of 1 second if nothing is received.
act on the command Set Log Level=<level>from the server to overwrite the filter log severity.
*/
void receiveThread(int fd)
{
	struct timeval complete_time;
	complete_time.tv_sec = 0;
	complete_time.tv_usec = 1;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &complete_time, sizeof(complete_time));

	while (isWorking)
	{
		memset(buf, 0, BUF_LEN);
		mutexes.lock();
		int msgSize = recvfrom(fd, buf, BUF_LEN, 0, (struct sockaddr*)&server_address, &addressLength);
		string message = buf;

		if (msgSize > 0)
		{
			string stat = message.substr((message.find("=") + 1));

			if (stat >= "DEBUG")
			{
				level_log = DEBUG;
			}

			else if (stat >= "WARNING")
			{
				level_log = WARNING;
			}

			else if (stat >= "ERROR")
			{
				level_log = ERROR;
			}

			else if (stat >= "CRITICAL")
			{
				level_log = CRITICAL;
			}

			else
			{
				continue;
			}
		}

		else
		{
			sleep(1);
		}

		message = "";
		mutexes.unlock();
	}
}