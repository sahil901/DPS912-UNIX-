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

	// Header that declares the thread class and the this_thread namespace 
#include <thread>

	// used to protect shared data from being simultaneously accessed by multiple threads
#include <mutex>

// makes available the type in_port_t and the type in_addr_t  
#include <arpa/inet.h> 

	// makes a socket available 
#include <sys/socket.h> 

	// defines miscellaneous symbolic constants and types, and declares miscellaneous functions 
#include <unistd.h> 

	// provides file stream classes
#include <fstream>

	// defines a number of requests and arguments for use by the functions fcntl() and open()
#include <fcntl.h>

	// defines the sockaddr_un structure
#include <sys/un.h>

	// includes a number of definitions for different types
#include <sys/types.h> 

	// defines a number of symbolic constants for use with waitpid()
#include <sys/wait.h>

// to avoid the use of using std:: each time
using namespace std;

// All global variables 
int fd;
const int BUF_LEN = 256, port = 1152;
char buf[BUF_LEN];
bool isWorking = true, inputWorking = true;
string ip_address = "127.0.0.1";
const char* f_path = "/tmp/fileLog";
struct sockaddr_in server_address, client_address;
socklen_t addressLength;
mutex mutexes;

// Forward Declarations
static void sigHandler(int signal);
void error(const char* errorMessage);
void setlevel_log();
void sendlevel_log(const char* level);
void readlevel_log();
void shutlevel_log();
void receiveThread(int t_fd); // diff parameters maybe


/*
The server main() function will create a non-blocking socket for UDP communications (AF_INET, SOCK_DGRAM).
The server main() function will bind the socket to its IP address and to an available port.
The server main() function will create a mutex and apply mutexing to any shared resources.
The server main() function will start a receive thread and pass the file descriptor to it.
*/
int main()
{
	// Declaring the Sigaction
	struct sigaction signal;
	signal.sa_handler = sigHandler;
	sigemptyset(&signal.sa_mask);
	signal.sa_flags = 0;
	int signalaction = sigaction(SIGINT, &signal, NULL);

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (fd < 0)
	{
		perror("An error occured: Socket is unable to be created.");
		exit(EXIT_FAILURE);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
	server_address.sin_port = htons(port);
	addressLength = sizeof(client_address);

	if (bind(fd, (const struct sockaddr*)&server_address, sizeof(server_address)) < 0)
	{
		perror("An error occured: Socket could not be binded.");
		exit(EXIT_FAILURE);
	}

	thread threadReceiver(receiveThread, fd);
	threadReceiver.detach();


	while (inputWorking)
	{
		cout << " ========== Logging Server ========== " << endl;
		cout << " ----- Please Select an Option ----- " << endl;
		cout << "1. Set Log Level" << endl;
		cout << "2. Dump the log file here" << endl;
		cout << "0. Shutdown" << endl;
		cout << "Input: ";

		int user_choice;
		cin >> user_choice;

		switch (user_choice)
		{
		case (1):
			setlevel_log();
			break;
		case(2):
			readlevel_log();
			break;
		case(0):
			shutlevel_log();
			break;
		default:
			cout << "The user choice was invalid! Please choose between 0, 1, 2" << endl;
		}
	}
	return 0;
}

/*
The server will shutdown gracefully via a ctrl-C via a shutdownHandler
*/
static void sigHandler(int signal)
{
	switch (signal)
	{
	case SIGINT:
		cout << " The CTRL-C Signal has been received!" << endl;
		isWorking = false;
		inputWorking = false;
		break;

	case SIGTSTP:
		cout << " The CTRL-Z Signal has been received!" << endl;
		break;

	default:
		cout << " Encountered a undefined signal!" << endl;
	}
}

/*
Performs error handling and then exit's if any error is found
*/
void error(const char* errorMessage)
{
	perror(errorMessage);
	exit(EXIT_FAILURE);
}

/*
The user will be prompted to enter the filter log severity.
The information will be sent to the logger.
*/
void setlevel_log()
{
	cout << "Please select from one of the options below:" << endl;
	cout << "1. DEBUG" << endl;
	cout << "2. WARNING" << endl;
	cout << "3. ERROR" << endl;
	cout << "4. CRITICAL" << endl;
	cout << "0. Return to Main Menu" << endl;

	cout << " User Choice: ";

	int user_choice;
	cin >> user_choice;

	switch (user_choice)
	{
	case(0):
		cout << "Return to Main Menu" << endl;
		break;
	case(1):
		sendlevel_log("DEBUG");
		break;
	case(2):
		sendlevel_log("WARNING");
		break;
	case(3):
		sendlevel_log("ERROR");
		break;
	case(4):
		sendlevel_log("CRITICAL");
		break;
	default:
		cout << " The user choice was invalid! Please choose between 0, 1, 2, 3, 4 for a valid entry." << endl;
	}
}

void sendlevel_log(const char* level)
{
	memset(buf, 0, BUF_LEN);
	int length = sprintf(buf, "Setting Log to Level=%s", level) + 1;
	cout << "Sending to Client: " << buf << endl;
	sendto(fd, buf, length, 0, (struct sockaddr*)&client_address, addressLength);
}

/*
The server will open its server log file for read only.
It will read the server log file contents and display them on the screen.
On completion, it will prompt the user with: "Press any key to continue:"
*/
void readlevel_log()
{
	FILE* readFile = fopen(f_path, "r");

	if (readFile == NULL)
	{
		error("File cannot be opened!");
	}

	char* line = NULL;
	size_t length = 0;

	while ((getline(&line, &length, readFile)) != -1)
	{
		cout << line << endl;
	}
	fclose(readFile);
}

/*
The receive thread will be shutdown via an is_running flag.
The server will exit its user menu.
The server will join the receive thread to itself so it doesn�t shut down before the receive thread does.
*/
void shutlevel_log()
{
	cout << "The system is shutting down!" << endl;
	isWorking = false;
	inputWorking = false;
}

/*
open the server log file for write only with permissions rw-rw-rw-
run in an endless while loop via an is_running flag.
apply mutexing to any shared resources used within the recvfrom() function.
ensure the recvfrom() function is non-blocking with a sleep of 1 second if nothing is received.
take any content from recvfrom() and write to the server log file.
*/
void receiveThread(int t_fd)
{
	struct timeval complete_time;
	complete_time.tv_sec = 0;
	complete_time.tv_usec = 1;
	setsockopt(t_fd, SOL_SOCKET, SO_RCVTIMEO, &complete_time, sizeof(complete_time));

	while (isWorking)
	{
		memset(buf, 0, BUF_LEN);

		mutexes.lock();
		FILE* log_file = fopen(f_path, "a");

		int size = recvfrom(t_fd, buf, BUF_LEN, 0, (struct sockaddr*)&client_address, &addressLength);

		string message = buf;

		cout << size << endl;

		if (size > 0) {
			fprintf(log_file, "%s", message.c_str());
		}
		else {
			sleep(1);
		}
		fclose(log_file);
		mutexes.unlock();
	}
}