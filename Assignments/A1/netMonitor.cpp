// Created by: Sahil Patel, Abdulbasid Guled and Andre Jenah

// USED PREVIOUS LABS AND MULTIPLE FILES OFF THE COURSE WEBSITE AS A REFERENCE: https://mwatler.github.io/unx511/index.html#

// header files 
	// basic input and output services 
#include <iostream>

// defines one variable type, one macro, and various functions for manipulating arrays of characters
#include <string.h>

//
#include <fstream>

// 
#include <cstring>

// constructs a vector, initializing its contents
#include <vector>

// specifies how program handles signals while it executes
#include <signal.h>

// 
#include <typeinfo>

//
#include <fcntl.h>

// 
#include <sys/un.h>

//
#include <sys/types.h>

// 
#include <sys/socket.h>

//
#include <unistd.h>

// don't have to use std:: each time
using namespace std;

// global variables 
const int BUF_LEN = 256;
bool isWorking = true;
bool isParent = true;

// temporary socket file in the /tmp directory
char socket_path[] = "/tmp/A1";

static void signalHandler(int signum)
{
	switch (signum)
	{
	case SIGINT:
		cout << "SignalHandler(" << signum << "): SIGINT" << endl;
		isWorking = false; 
		break; 
	default:
		cout << "SignalHandler(" << signum << "): unknown" << endl;
	}
}

void handleError() {
	cerr << "ERROR: " << strerror(errno) << "!" << endl;
	unlink(socket_path);
	exit(-1);
}

string receiveMessage(int fd, char* buffer) {
	bzero(buffer, 255);
	read(fd, buffer, 255);
	string message = buffer;
	return message;
}

void sendMessage(int fd, string message) {
	write(fd, message.c_str(), message.length() + 1);
}

int main()
{
	struct sockaddr_un address, clientSocket;
	char buffer_recieve[BUF_LEN];
	int len, ret, retRecv, numInterfaces, m_fd;
	fd_set working_fd;
	int n_clients = 0;
	vector<string> interfaceNames;
	socklen_t clientLen;

	FD_ZERO(&working_fd);

	struct sigaction do_act;
	do_act.sa_handler = signalHandler;
	sigemptyset(&do_act.sa_mask);
	do_act.sa_flags = 0;
	sigaction(SIGINT, &do_act, NULL);


	// Get number of interfaces and names from the user

	while (true) {
		cout << "How many interfaces should we monitor? ";
		cin >> numInterfaces;
		for (int i = 0; i < numInterfaces; ++i) {
			string name;
			cout << "Enter an interface: ";
			cin >> name;
			interfaceNames.push_back(name);
		}
		break;
	}
	pid_t childPid[numInterfaces];
	int connectedFDs[numInterfaces];

	// For testing purposes, only 2 interfaces will exist so we only input 2
	const char* intf[] = { interfaceNames[0].c_str(), interfaceNames[1].c_str() };


  // Creates child processes to run interface monitor
	for (int i = 0; i < numInterfaces & isParent; ++i)
	{
		childPid[i] = fork();

		if (childPid[i] == 0)
		{
			isParent = false; 
			execlp("./intfMonitor", "./intfMonitor", intf[i], NULL);
			cout << strerror(errno) << endl;
		}
	}

  // Setup Sever Socket
	if ((m_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) 
	{
		handleError();
	}

	memset(&address, 0, sizeof(address));
	address.sun_family = AF_UNIX;
	strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);
	unlink(socket_path);
	len = strlen(address.sun_path) + sizeof(address.sun_family);

	if ((bind(m_fd, (struct sockaddr*)&address, len)) < 0) 
	{
		close(m_fd);
		handleError();
	}

	cout << "Waiting for the client..." << endl;
	
 // Listen, accept connections, send ready command
	for (int i = 0 ; i < numInterfaces; ++i)
	{
		listen(m_fd, 5);
		clientLen = sizeof(clientSocket);
		retRecv = accept(m_fd, (struct sockaddr*)&clientSocket, &clientLen);
		FD_SET(retRecv, &working_fd);
		connectedFDs[i] = retRecv;

		string message = receiveMessage(retRecv, buffer_recieve);
		if (message != "Ready") {
			cout << "ERROR: Not ready to start yet" << endl;
		}

		// Send Monitor command to interface monitors to start
		sendMessage(retRecv, "Monitor");

		// Receive confirmation
		message = receiveMessage(retRecv, buffer_recieve);
		if(message != "Monitoring"){
            cout << "Error: Monitoring failed" << endl;
            sendMessage(retRecv, "Shut Down");
        }
	}

	// Handle Link Down and Link Up commands
	do {
		string message = receiveMessage(retRecv, buffer_recieve);

		if (strcmp(message.c_str(), "Link down") == 0) {
			sendMessage(retRecv, "Set Link Up");
		} else if (strcmp(message.c_str(), "Link Up") == 0) {
			sendMessage(retRecv, "Monitor");
		} else {
			continue; // Loop will continue until Network Monitor receives SIGINT, to shut down
		}
	} while (isWorking);

	// Shut down process
	for (int i = 0; i < numInterfaces; ++i) 
	{
		cout << "server: request client " << i + 1 << " to quit" << endl;
		
		sendMessage(connectedFDs[i], "Shut Down");

		string message = receiveMessage(connectedFDs[i], buffer_recieve);

		if (strcmp(message.c_str(), "Done") == 0) {
			close(connectedFDs[i]);
		}
	}

	close(m_fd);
	unlink(socket_path);
	return 0;
}