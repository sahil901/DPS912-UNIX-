// Sahil Patel 
// Used course notes as a reference

// used code from https://mwatler.github.io/unx511/Week8/select5/server.cpp

// HEADER FILES
#include <string.h>
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
    struct sockaddr_un addr, c_addr;
    char buf[BUF_LEN];
    int fd, rc, y_connection;
    bool is_working = true;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) 
    {
        e_error("The socket could not be created.");
    }

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path));
    unlink(socket_path);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) 
    {
        close(fd);
        perror(strerror(errno));
    }

    listen(fd, 2);
    socklen_t CADDRESS_SIZE = sizeof(c_addr);
    cout << "Waiting for the client..." << endl;

    y_connection = accept(fd, (struct sockaddr*)&c_addr, &CADDRESS_SIZE);

    if (y_connection < 0)
    {
        cout << "No connection could be established." << endl;
       e_error(strerror(errno));
    }

    // gotten this from the image
    cout << "The server requests the client's pid" << endl;
    rc = write(y_connection, c_pid.c_str(), c_pid.size());

    if (rc < 0) 
    {
        e_error(strerror(errno));
    }

    bzero(buf, BUF_LEN);


    rc = read(y_connection, buf, BUF_LEN - 1);

    if (rc < 0) 
    {
        e_error(strerror(errno));
    }

    // gotten this from the image
    else 
    {
        cout << "This client has pid: " << buf << endl;
    }

    // gotten this from the image
    cout << "The server requests the client to sleep" << endl;
    rc = write(y_connection, c_sleep.c_str(), c_sleep.size());

    bzero(buf, BUF_LEN);

    rc = read(y_connection, buf, BUF_LEN - 1);

    if (rc < 0) 
    {
        e_error(strerror(errno));
    }

    else 
    {
        if (strcmp(buf, "Done") == 0) 
        {
              // gotten this from the image
            cout << "The server requests the client to quit" << endl;
            rc = write(y_connection, c_quit.c_str(), c_quit.size());
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

    unlink(socket_path);
    close(y_connection);
    close(fd);
    return EXIT_SUCCESS;
}