// Sahil Patel (159-065-176)

// Used your website as a reference 

/* 
How does dup() work ?
- The dup() creates a copy of a file descriptor and then utilizes the lowest number that
has not been used for the descriptor. Once the copy is created without any issues then both 
the original and copied file descriptors can be used conversely. Since they both are refering to the same 
open file desc they share the same file status flags and offset. 


How does dup2() work ?
- The dup2() is quite similar to dup() but the only difference here is that instead of utilizing the 
lowest number that has not been used for the descriptor, it used the descriptor that has been specified 
by the user. Also, if the file descriptor newfd was open then it will be closed before its used again. It closing 
is done automatically.


How does execlp() work ?
- The execlp() duplicates the shell in searching for a exec file if the filename that is specified doesn't have a slash. 
The file should in a colon seperated list in the PATH var and if it's not defined then the path lists defaults to a list 
that includes the directories.

*/



#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>

using namespace std;

// from your code 
const int LEN = 64;

void errorDetection(const char* e_mess) 
{
    perror(e_mess);
    exit(EXIT_FAILURE);
}


int main(int argc, char* argv[]) {

    if (argc < 2)
    {
        errorDetection("There must be a minimum of 2 arguments that must be passed.");
    }

    int pid_fd[2];
    int pid_stat = -1;
    pid_t pid = 0, first_pid, second_pid;

    // from your code 
    char argument1[LEN];
    char argument2[LEN];
    strcpy(argument1, argv[1]);
    strcpy(argument2, argv[2]);
    char arg1[3][LEN];//max 3 strings
    char arg2[3][LEN];//max 3 strings
    int len1 = 0, len2 = 0;

    // from your code 
    char* token = strtok(argument1, " ");
    while (token != NULL) 
    {
        strcpy(arg1[len1], token);
        token = strtok(NULL, " ");
        ++len1;
    }
    token = strtok(argument2, " ");

    // from your code 
    while (token != NULL) 
    {
        strcpy(arg2[len2], token);
        token = strtok(NULL, " ");
        ++len2;
    }

    char* first_argument[len1 + 1];
    char* second_argument[len2 + 1];

    for (int i = 0; i < len1 + 1; i++) 
    {
        if (i == len1) 
        {
            first_argument[i] = (char*)NULL;
        }
        else 
        {
            first_argument[i] = arg1[i];
        }
    }

    for (int i = 0; i < len2 + 1; i++) 
    {
        if (i == len2) 
        {
            second_argument[i] = (char*)NULL;
        }
        else 
        {
            second_argument[i] = arg2[i];
        }
    }

    if (pipe(pid_fd) == -1)
    {
        errorDetection("An error has occured: the pipe has failed.");
    }

    first_pid = fork();
    if (first_pid == 0) 
    {
        if (pid_fd[1] != STDOUT_FILENO) 
        {
            if (dup2(pid_fd[1], STDOUT_FILENO) == -1) 
            {
                errorDetection("An error has occured: the dup2 has failed (first).");
            }
            if (close(pid_fd[0]) == -1)
            {
                errorDetection("An error has occured: the close has failed.");
            }
            if (close(pid_fd[1]) == -1)
            {
                errorDetection("An error has occured: the close has failed.");
            }
        }
 
        execvp(first_argument[0], first_argument);

        errorDetection("execvp"); 

    }
    
    second_pid = fork();
    if (second_pid == 0) 
    {
        if (pid_fd[0] != STDIN_FILENO)
        {
            if (dup2(pid_fd[0], STDIN_FILENO) == -1) 
            {
                errorDetection("An error has occured: the dup2 has failed (second).");
            }
            if (close(pid_fd[0]) == -1) 
            {
                errorDetection("An error has occured: the close has failed.");
            }
            if (close(pid_fd[1]) == -1) 
            {
                errorDetection("An error has occured: the close has failed.");
            }
        }

        execvp(second_argument[0], second_argument);

        errorDetection("execvp"); 
    }

    if (close(pid_fd[0]) == -1) 
    {
        errorDetection("An error has occured: the close has failed.");
    }

    if (close(pid_fd[1]) == -1) 
    {
        errorDetection("An error has occured: the close has failed.");
    }

    while (pid > 0) 
    {
        pid = wait(&pid_stat);
    }

    return 0;
}
