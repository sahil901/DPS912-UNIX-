// Sahil Patel 
// Used course notes as a reference 

// used code from https://mwatler.github.io/unx511/Week8/select5/client.cpp

// HEADER FILES
#include <sys/socket.h>
#include <iostream>
#include <stdio.h>
#include <sys/un.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// TO REMOVE STD::
using namespace std;

const char socket_path[] = "/tmp/lab6";
string c_pid = "Pid", c_sleep = "Sleep", c_quit = "Quit";

void e_error(const char* message) 
{
    perror(message);
    exit(EXIT_FAILURE);
}


int main(int argc, char* argv[])
{
    const int BUF_LEN = 256;
    struct sockaddr_un addr;
    char buf[BUF_LEN];
    int fd, rc, connection;
    bool is_working = true;

    memset(&addr, 0, sizeof(addr));


    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) 
    {
        e_error(strerror(errno));
    }

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    while (is_working)
    {
        // Connect socket to server socket
        if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) 
        {
            close(fd);
            e_error(strerror(errno));
        }

        bzero(buf, BUF_LEN);
        rc = read(fd, buf, BUF_LEN - 1);

        if (rc < 0) 
        {
            e_error("An error has occured, the reading has failed.");
            return -1;
        }

        else 
        {
            // gotten this from the image in the project
            cout << "An request for the clients pid has been recieved." << endl;
            if (strcmp(buf, c_pid.c_str()) == 0) 
            {
                int pid = getpid();
                string response = to_string(pid);
                rc = write(fd, response.c_str(), sizeof(pid));
            }

            else 
            {
                e_error("An error has occured, a command that does not work has been issued.");
            }

            if (rc < 0) 
            {
                e_error("An error has occured, the writing has failed.");
            }
        }

        bzero(buf, BUF_LEN);
        rc = read(fd, buf, BUF_LEN - 1);

        if (rc < 0)
        {
            e_error(strerror(errno));
        }
        
        else 
        {

          //  The client will sleep for 5 seconds then respond to the server with “Done”.
            if (strcmp(buf, c_sleep.c_str()) == 0)
            {
                // taken this from the image provided on the website
                cout << "This client is going to sleep for 5 seconds..." << endl;
                sleep(5);
                rc = write(fd, "Done", 5);
            }

            else 
            {
                e_error(strerror(errno));
            }

            if (rc < 0) 
            {
                e_error(strerror(errno));
            }
        }

        bzero(buf, BUF_LEN);
        rc = read(fd, buf, BUF_LEN - 1);

        if (rc < 0) 
        {
            e_error(strerror(errno));
        }

        else 
        {
            if (strcmp(buf, c_quit.c_str()) == 0) 
            {
                // taken this from the image provided on the website
                cout << "This client is quitting." << endl;
                is_working= false;
            }

            else
            {
                e_error(strerror(errno));
            }

            if (rc < 0) 
            {
                e_error(strerror(errno));
            }
        }
    }
    close(connection);
    close(fd);
    return EXIT_SUCCESS;
}