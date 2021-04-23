// Sahil Patel (159-065-176)

/*
What is the difference between synchronous and asynchronous communication?
- Synchronous communication occurs in real time meaning its faster and more dynamic. Meanwhile, asynchronous communication happens over time so you have control over the speed the communication happens. 

Do you prefer socket reads in a receive thread or do you prefer both socket read and write to be in the main() function? Why?
- Personally, I prefer the socket read in the receive thread.
*/

#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <queue>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <pthread.h>


// to avoid the use of using std:: each time
using namespace std;

// All global variables 
const int BUF_LEN = 256;
const int C_MAX = 3;
int C_CNT = 0;
bool is_Working = true;
char buf[BUF_LEN];
struct sockaddr_in server_address, client_address;
int fd_master, fd_max, fd_client[C_MAX];
socklen_t socketLength;
queue<string> mesQ;
fd_set activeFDs, readFDs;
pthread_mutex_t serverMutex;

// declaring receiver function
void* receiver(void* args);

// the reciever function 
void* receiver(void* args)
{
    char buf[BUF_LEN];

    struct timeval close_time;

    close_time.tv_sec = 5;
    close_time.tv_usec = 0;

    while (is_Working)
    {
        memset(buf, 0, BUF_LEN);
        readFDs = activeFDs;

        int s = select(fd_max + 1, &readFDs, NULL, NULL, &close_time);

        if (s < 0)
        {
            cerr << "Server Error Occured: " << strerror(errno) << endl;
        }
        else
        {
            if (FD_ISSET(fd_master, &readFDs))
            {
                fd_client[C_CNT] = accept(fd_master, NULL, NULL);

                if (fd_client[C_CNT] < 0)
                {
                    cout << "SERVER: " << strerror(errno) << endl;
                }

                else
                {
                    FD_SET(fd_client[C_CNT], &activeFDs);

                    if (fd_max < fd_client[C_CNT])
                    {
                        fd_max = fd_client[C_CNT];
                    }

                    C_CNT += 1;
                }
            }

            else if (s == 1)
            {
                for (int i = 0; i < C_CNT; ++i)
                {
                    if (FD_ISSET(fd_client[i], &readFDs))
                    {
                        s = recv(fd_client[i], buf, BUF_LEN, 0);

                        if (s == -1)
                        {
                            cout << "Error Occured: Read Error" << endl;
                            cout << strerror(errno) << endl;
                        }

                        string message = buf;

                        pthread_mutex_lock(&serverMutex);
                        mesQ.push(message);
                        pthread_mutex_unlock(&serverMutex);
                    }
                }
            }

            else
            {
                is_Working = false;
            }
        }
    }
}

// main program
int main(int argc, char* argv[]) 
{

    if (argc != 2) 
    {
        cerr << "Error Occured! The arugment can only accept a num value as an argument." << endl;
        return -1;
    }

    const int local_connect = atoi(argv[1]);

    FD_ZERO(&activeFDs);
    FD_ZERO(&readFDs);

    fd_master = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(local_connect);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd_master, (const struct sockaddr*)&server_address, sizeof(server_address)) < 0) 
    {
        perror("Error Occured! The socket can't be binded.");
        exit(EXIT_FAILURE);
    }

    listen(fd_master, 1);
    cout << "Listening on local_connect " << local_connect << endl;

    FD_SET(fd_master, &activeFDs);
    fd_max = fd_master;

    pthread_t threads;

    int s = pthread_create(&threads, NULL, receiver, &fd_master);
   
    if (s != 0) 
    {
        cerr << "Error Occured! The thread can't be started, system exiting." << endl;
        cerr << strerror(errno) << endl;
        close(fd_master);
        return -1;
    }

    while (is_Working) 
    {
        if (mesQ.size() == 0) 
        {
            sleep(1);
        }

        while (mesQ.size() > 0) 
        {
            pthread_mutex_lock(&serverMutex);
            string recieveMessage = mesQ.front();
            mesQ.pop();
            pthread_mutex_unlock(&serverMutex);

            cout << "The server has received the following message: " << recieveMessage << endl;
        }
    }

    cout << "NOTICE: The system is shutting down." << endl;

    for (int i = 0; i < C_CNT; ++i) 
    {
        send(fd_client[i], "Quit", 5, 0);
    }

    pthread_join(threads, NULL);

    close(fd_master);
    return 0;
}
