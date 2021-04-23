/* Sahil Patel (159-065-176)
Used some of your code examples

Questions:
Compare and contrast the mechanisms of inter-process communication that you have studied so far.

1. Between sockets, pipes, fifos, and messages, which is your favorite and why?
- My favourite is pipes since i have the most experience with them. I find them easy to use. 

2. Which is your least favorite and why?
- My least fav is message queues since i just learned it now and im not tooo familair with it.

3.For the scenario presented in this lab, is there a need for a server? Why?
- For this specifc lab, the simple answer is no. The clients are able to contact each other by using the message queques which gets rid of the need of the server.  

*/

// Header files
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <queue>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "client.h"

// too avoid the use of std::
using namespace std;

// global vars
int id_message;
bool isWorking;
key_t one;
queue<Message> message_q;
pthread_mutex_t m_server;

void* recv(void* arg);
static void sigHandler(int signal);

int main()
{
    int first, second, len;
    char buff[BUF_LEN];
    Message message_recieve, message_send;

    struct sigaction act_sig;
    act_sig.sa_handler = sigHandler;
    sigemptyset(&act_sig.sa_mask);
    act_sig.sa_flags = 0;

    int signalAction = sigaction(SIGINT, &act_sig, NULL);

    if (signalAction < 0) 
    {
        cout << "An error occured: The handler was unable to be registed.registering handler." << endl;
        return -1;
    }

    one = ftok("ServerLab8", 65);
    id_message = msgget(one, 0666 | IPC_CREAT);

    isWorking = true;

    pthread_t p_tid;
    second = pthread_create(&p_tid, NULL, recv, &first);
    if (second != 0)
    {
        cout << "An error occuted: Cannot create the thread." << endl;
        close(first);
        return -1;
    }

    cout << "It's ready to recieve the messages." << endl;

    while (isWorking) 
    {
        while (message_q.size() > 0) 
        {
            pthread_mutex_lock(&m_server);
            message_recieve = message_q.front();
            message_q.pop();
            pthread_mutex_unlock(&m_server);

            message_send.mtype = message_recieve.msgBuf.dest;
            message_send.msgBuf.source = message_recieve.msgBuf.source;
            message_send.msgBuf.dest = message_recieve.msgBuf.dest;

            sprintf(message_send.msgBuf.buf, message_recieve.msgBuf.buf, getpid());
            msgsnd(id_message, &message_send, sizeof(message_send), 0);
            sleep(1);
        }
    }

    message_send.msgBuf.source = 4;

    for (int i = 0; i < 3; ++i) 
    {
        message_send.mtype = i + 1;
        sprintf(message_send.msgBuf.buf, "Quit");
        msgsnd(id_message, &message_send, sizeof(message_send), 0);
    }

    pthread_join(p_tid, NULL);
    msgctl(id_message, IPC_RMID, NULL);

    return 0;
}

static void sigHandler(int signal) 
{
    switch (signal)
    {
    case SIGINT:
        cout << "A signal that is of SIGINT has been received." << endl;
        isWorking = false;
        break;
    default:
        cout << "A signal that is undefined has been received." << endl;
    }
}

void* recv(void* arg) 
{
    char buff[BUF_LEN];

    while (isWorking) 
    {
        Message mess;
        msgrcv(id_message, &mess, sizeof(mess), 4, 0);

        pthread_mutex_lock(&m_server);
        message_q.push(mess);
        pthread_mutex_unlock(&m_server);
    }
    pthread_exit(NULL);
}