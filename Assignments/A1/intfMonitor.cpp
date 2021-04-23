// Created by: Sahil Patel, Abdulbasid Guled and Andre Jenah

// header files 
	// basic input and output services 
#include <iostream>

	// allow the use of string data type 
#include <string>

	// defines one variable type, one macro, and various functions for manipulating arrays of characters
#include <string.h>

	// perform output and input of characters to/from files
#include <fstream>

	// constructs a vector, initializing its contents
#include <vector>

	// defines the following data type through typedef
#include <dirent.h>

	// performing input and output
#include <stdio.h>

	// performing general functions
#include <stdlib.h>

	// useful for testing and mapping characters
#include <ctype.h>

	// defines a collection of functions especially designed to be used on ranges of elements
#include <algorithm>

	// defines miscellaneous symbolic constants and types, and declares miscellaneous functions
#include <unistd.h>

	// specifies how program handles signals while it executes
#include <signal.h>

	// define the if_nameindex structure
#include <net/if.h>

	// 
#include <sys/ioctl.h>

	// 
#include <sys/socket.h>

	//
#include <sys/types.h>

	// 
#include <sys/un.h>

// don't have to use std:: each time
using namespace std;

// global variables 
const int BUF_LEN = 256; 
bool isWorking = true; 

// temporary socket file in the /tmp directory
char socket_path[] = "/tmp/A1"; 

// Send and receive message functions

string receiveMessage(int fd, char buffer[BUF_LEN]) {
	bzero(buffer, 255);
	read(fd, buffer, 255);
	string message = buffer;
	return message;
}

void sendMessage(int fd, string message) {
	write(fd, message.c_str(), message.length() + 1);
}

// Error handler function
void handleError() {
	cerr << "ERROR: " << strerror(errno) << "!" << endl;
	unlink(socket_path);
	exit(-1);
}

// Interface setup function to turn on the network interface

int interfaceUp(int fd, string iface) {
    struct ifreq ifr;

    memset(&ifr, 0, sizeof ifr);

    strncpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ);

    ifr.ifr_flags |= IFF_UP;
    return ioctl(fd, SIOCSIFFLAGS, &ifr);
}

// main 
int main(int argc, char* argv[])
{
	// variables 
	struct dirent* version_one = NULL;
	struct dirent* version_two = NULL;
	DIR* v0 = NULL;
	DIR* v1 = NULL;
	DIR* v2 = NULL;

	// statistics for each network interface can be found under here
	string stats_path = "/sys/class/net";
	string folders;
	string interface;

	// Socket communication
	struct sockaddr_un addr;
	char buffer_len[BUF_LEN];
	char buffer_recieve[BUF_LEN];
	int len, ret, retRecv; 
	int fd, length; 

	memset(&addr, 0, sizeof(addr));

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) 
	{
		handleError();
	}
	addr.sun_family = AF_UNIX;

	// Setting path of socket to local socket file 
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
	length = strlen(addr.sun_path) + sizeof(addr.sun_family);

	// establishing connection to the local socket 
	if (connect(fd, (struct sockaddr*) &addr, length) < 0) 
	{
		cout << "The Client " << getpid() << "  has the following error: " << strerror(errno) << endl;
		close(fd);
		exit(-1);
	}

  // We need two arugments so if the directory name doesn't exist, exit immediately
	if (argc != 2)
	{
		fprintf(stderr, "The usage is: %s dirname\n", argv[0]);
		return(1);
	}
	else
	{
		folders = argv[1];
		string done = stats_path + "/" + folders;
		v1 = opendir(done.c_str());

		if (v1 == NULL)
		{
			perror("The directory could not be opened.");
			return (2);
		}
	}

	v0 = opendir(stats_path.c_str());
	if (v0 == NULL)
	{
		perror("The directory could not be opened.");
		return (2);
	}

	sendMessage(fd, "Ready");

  // Infinite loop will read in all values, and send them to the network monitor
	do
	{
		vector<string> allPaths;
		interface = folders;
		string message;

    // Get all the variables to send to networkMonitor according to requirements.
		string operstatePath = stats_path + "/" + folders + "/operstate";
		std::ifstream openOperState(operstatePath);
		
		string operstate((std::istreambuf_iterator<char>(openOperState)),
		std::istreambuf_iterator<char>());
		operstate.erase(std::remove(operstate.begin(), operstate.end(), '\n'), operstate.end());

		string carrierUpCountPath = stats_path + "/" + folders + "/carrier_up_count";
		std::ifstream openCarrierUpCount(carrierUpCountPath);
		string carrierUpCount((std::istreambuf_iterator<char>(openCarrierUpCount)),
		std::istreambuf_iterator<char>());
		carrierUpCount.erase(std::remove(carrierUpCount.begin(), carrierUpCount.end(), '\n'),carrierUpCount.end());

		string carrierDownCountPath = stats_path + "/" + folders + "/carrier_down_count";
		std::ifstream openCarrierDownCount(carrierDownCountPath);
		string carrierDownCount((std::istreambuf_iterator<char>(openCarrierDownCount)),
		std::istreambuf_iterator<char>());
		carrierDownCount.erase(std::remove(carrierDownCount.begin(), carrierDownCount.end(), '\n'),carrierDownCount.end());

		string rxbytesPath = stats_path + "/" + folders + "/statistics/rx_bytes";
		std::ifstream openRxbytes(rxbytesPath);
		string rxbytes((std::istreambuf_iterator<char>(openRxbytes)), std::istreambuf_iterator<char>());
		rxbytes.erase(std::remove(rxbytes.begin(), rxbytes.end(), '\n'), rxbytes.end());

		string rxdroppedPath = stats_path + "/" + folders + "/statistics/rx_dropped";
		std::ifstream openRxdropped(rxdroppedPath);
		string rxDropped((std::istreambuf_iterator<char>(openRxdropped)), std::istreambuf_iterator<char>());
		rxDropped.erase(std::remove(rxDropped.begin(), rxDropped.end(), '\n'), rxDropped.end());

		string rxErrorsPath = stats_path + "/" + folders + "/statistics/rx_errors";
		std::ifstream openRxErrors(rxErrorsPath);
		string rxErrors((std::istreambuf_iterator<char>(openRxErrors)),std::istreambuf_iterator<char>());
		rxErrors.erase(std::remove(rxErrors.begin(), rxErrors.end(), '\n'),rxErrors.end());

		string rxPacketsPath = stats_path + "/" + folders + "/statistics/rx_packets";
		std::ifstream openRxPackets(rxPacketsPath);
		string rxPackets((std::istreambuf_iterator<char>(openRxPackets)),std::istreambuf_iterator<char>());
		rxPackets.erase(std::remove(rxPackets.begin(), rxPackets.end(), '\n'),rxPackets.end());

		string txBytesPath = stats_path + "/" + folders + "/statistics/tx_bytes";
		std::ifstream opentxBytes(txBytesPath);
		string txBytes((std::istreambuf_iterator<char>(opentxBytes)),std::istreambuf_iterator<char>());
		txBytes.erase(std::remove(txBytes.begin(), txBytes.end(), '\n'),txBytes.end());

		string txDroppedPath = stats_path + "/" + folders + "/statistics/tx_dropped";
		std::ifstream openTxDroppedPath(txDroppedPath);
		string txDropped((std::istreambuf_iterator<char>(openTxDroppedPath)),std::istreambuf_iterator<char>());
		txDropped.erase(std::remove(txDropped.begin(), txDropped.end(), '\n'),txDropped.end());

		string txErrorsPath = stats_path + "/" + folders + "/statistics/tx_errors";
		std::ifstream openTxErrors(txErrorsPath);
		string txErrors((std::istreambuf_iterator<char>(openTxErrors)),std::istreambuf_iterator<char>());
		txErrors.erase(std::remove(txErrors.begin(), txErrors.end(), '\n'),txErrors.end());

		string txPacketsPath = stats_path + "/" + folders + "/statistics/tx_packets";
		std::ifstream openTxPackets(txPacketsPath);
		string txPackets((std::istreambuf_iterator<char>(openTxPackets)),std::istreambuf_iterator<char>());
		txPackets.erase(std::remove(txPackets.begin(), txPackets.end(), '\n'),txPackets.end());

		message = receiveMessage(fd, buffer_recieve); // Wait for Monitor message before displaying data

		if (strcmp(message.c_str(), "Monitor") == 0)
		{
			sendMessage(fd, "Monitoring");

			bool isRunning = true;
      
      bool isDown = operstate == "down";

			do {
				cout << "Name of Interface: " << interface << " Operstate: " << operstate << " Carrier up: " << carrierUpCount << " Carrier down:" << carrierDownCount << endl;

				cout << "rx_bytes: " << rxbytes << " rx_dropped: " << rxDropped << " rx_errors: " << rxErrors << " rx_packets:" << rxPackets << endl;

				cout << "tx_bytes: " << txBytes << "tx_dropped: " << txDropped << "tx_errors: " << txErrors << "tx_packets: " << txPackets << endl;

				cout << "\n" << endl;

				sleep(1);
      
			} while (isRunning && !isDown); // As long as the operstate is not down, continue to display data
      
        
        if (operstate == "down") {
				sendMessage(fd, "Link down");
			}
		}
   
   // Call interfaceUp function to set the network link back up

		else if (strcmp(message.c_str(), "Set Link Up") == 0)
		{
			int n = interfaceUp(fd, argv[1]);
			if (n < 0) {
				handleError();
			} else {
				sendMessage(fd, "Link Up");
			}
		}
   
   // When a shutdown message is received, send a message to the networkMonitor, close connection socket, and exit out of the loop

		else if (strcmp(message.c_str(), "Shut Down") == 0)
		{
			cout << "The client " << getpid() << " has recieved a request to quit" << endl;
			sendMessage(fd, "Done");
			close(fd);
			isWorking = false;
		}
		else {
			cout << "Command not known" << endl;
		}
	} while (isWorking);
 
	closedir(v0);
	return (0);
}