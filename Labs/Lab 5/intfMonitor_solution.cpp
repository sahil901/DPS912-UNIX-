//intfMonitor_solution.cpp - An interface monitor
//
// 13-Jul-20  M. Watler         Created.

// Sahil Patel 
// Used course notes as a reference 

#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

const int MAXBUF = 128;
bool isRunning = false;

//TODO: Declare your signal handler function prototype
static void signalHandler(int signal); 

int main(int argc, char* argv[])
{
	//TODO: Declare a variable of type struct sigaction
	//      For sigaction, see http://man7.org/linux/man-pages/man2/sigaction.2.html
	struct sigaction mon_solu;
	mon_solu.sa_handler = signalHandler;
	sigemptyset(&mon_solu.sa_mask);
	mon_solu.sa_flags = 0;



	char interface[MAXBUF];
	char statPath[MAXBUF];
	const char logfile[] = "Network.log";//store network data in Network.log
	int retVal = 0;

	//TODO: Register signal handlers for SIGUSR1, SIGUSR2, ctrl-C and ctrl-Z
	
	int s_o = sigaction(SIGUSR1, &mon_solu, NULL);
	int s_t = sigaction(SIGUSR2, &mon_solu, NULL);
	int s_tt = sigaction(SIGINT, &mon_solu, NULL);
	int s_f = sigaction(SIGTSTP, &mon_solu, NULL);
	
	//TODO: Ensure there are no errors in registering the handlers
	if (s_o < 0 || s_t < 0 || s_tt < 0 || s_f < 0)
	{
		cerr << "The signal handler(s) are not working/registering." << endl;
		return false;
	}




	strncpy(interface, argv[1], MAXBUF);//The interface has been passed as an argument to intfMonitor
	int fd = open(logfile, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
	cout << "intfMonitor:main: interface:" << interface << ":  pid:" << getpid() << endl;

	//TODO: Wait for SIGUSR1 - the start signal from the parent
	pause();


	while (isRunning) {
		//gather some stats
		int tx_bytes = 0;
		int rx_bytes = 0;
		int tx_packets = 0;
		int rx_packets = 0;
		ifstream infile;
		sprintf(statPath, "/sys/class/net/%s/statistics/tx_bytes", interface);
		infile.open(statPath);
		if (infile.is_open()) {
			infile >> tx_bytes;
			infile.close();
		}
		sprintf(statPath, "/sys/class/net/%s/statistics/rx_bytes", interface);
		infile.open(statPath);
		if (infile.is_open()) {
			infile >> rx_bytes;
			infile.close();
		}
		sprintf(statPath, "/sys/class/net/%s/statistics/tx_packets", interface);
		infile.open(statPath);
		if (infile.is_open()) {
			infile >> tx_packets;
			infile.close();
		}
		sprintf(statPath, "/sys/class/net/%s/statistics/rx_packets", interface);
		infile.open(statPath);
		if (infile.is_open()) {
			infile >> rx_packets;
			infile.close();
		}
		char data[MAXBUF];
		//write the stats into Network.log
		int len = sprintf(data, "%s: tx_bytes:%d rx_bytes:%d tx_packets:%d rx_packets: %d\n", interface, tx_bytes, rx_bytes, tx_packets, rx_packets);
		write(fd, data, len);
		sleep(1);
	}
	close(fd);

	return 0;
}

//TODO: Create a signal handler that starts your program on SIGUSR1 (sets isRunning to true),
//      stops your program on SIGUSR2 (sets isRunning to false),
//      and discards any ctrl-C or ctrl-Z.
//
//      If the signal handler receives a SIGUSR1, the following message should appear on the screen:
//      intfMonitor: starting up
//
//      If the signal handler receives a ctrl-C, the following message should appear on the screen:
//      intfMonitor: ctrl-C discarded
//
//      If the signal handler receives a ctrl-Z, the following message should appear on the screen:
//      intfMonitor: ctrl-Z discarded
//
//      If the signal handler receives a SIGUSR2, the following message should appear on the screen:
//      intfMonitor: shutting down
//
//      If the signal handler receives any other signal, the following message should appear on the screen:
//      intfMonitor: undefined signal


static void signalHandler(int signal)
{
	switch (signal) 
	{
	case SIGUSR1:
		cout << "intfMonitor: starting up" << endl;
		isRunning = true;
		break;
	case SIGINT:
		cout << "intfMonitor: ctrl-C discarded" << endl;
		break;
	case SIGTSTP:
		cout << "intfMonitor: ctrl-Z discarded" << endl;
		break;
	case SIGUSR2:
		cout << "intfMonitor: shutting down" << endl;
		isRunning = false;
		break;
	default:
		cout << "intfMonitor: undefined signal" << endl;
		break;
	}
}