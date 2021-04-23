// Created By: Sahil Patel (159-065-176)
// Source Code & Further info gathered from: https://mwatler.github.io/unx511/Labs/Lab2/pidUtil.h and from instructions as it literally told me exactly what to do...

// Header File(s)
#include <iostream>
#include <vector>
#include "pidUtil.h"

// to not use std::
using namespace std;

// main code of the program
int main() 
{
    // creation of variables 
        
    // error status 
    ErrStatus err_status;

    // error message 
    string err_message = "";
    
    // vector for PID
    vector<int> v_pid;

    // hold temporary string
    string s_name = "Name was not found";

    // setting getAllpids to error_status
    // call GetAllPids and store v_pid inside and set it to err_status
    err_status = GetAllPids(v_pid);

    // printing out all pids and their names.
    // if error status is equal to 0 then do the following
    if (err_status == 0)
    {
        // could not do the reg for loop (change for vector)
        // iterates through 
        for (auto j : v_pid) 
        {
            // get name by using GetNameByPid for each instance of i and set it to err_status 
            // call GetNameByPid()
            err_status = GetNameByPid(j, s_name);
          
            // if error status is equal to 0 then output the following (no error)
            if (err_status == 0) 
            {
               cout << "PID: " << j << " Name: " << s_name << endl;
            }

            // else get the err_status and set it to err_message
            else 
            {
                err_message = GetErrorMsg(err_status);
            }
        }
    }

    // Setting pid to 1
    int pid = 1;

    // calling GetNameByPid() 
    err_status = GetNameByPid(pid, s_name);

    //  printing out the name of pid 1
    // if error status is equal to 0 then output the following (no error)
    if (err_status == 0)
    {
        cout << "PID: " << pid << " Name: " << s_name << endl;
    }

    // else get the err_status and set it to err_message
    else 
    {
        err_message = GetErrorMsg(err_status);
    }

    // setting name to "Lab2"
    s_name = "Lab2";

    // Calling GetPidByName() to get the pid of Lab2
    err_status = GetPidByName(s_name, pid);

    // Printing "Lab2" and the pid of Lab2
    // if error status is equal to 0 then output the following (no error)
    if (err_status == 0) 
    {
        cout << "PID: " << pid << " Name: " << s_name << endl;
    }

    // else get the err_status and set it to err_message
    else 
    {
        err_message = GetErrorMsg(err_status);
    }

    // setting name to "Lab22"
    s_name = "Lab22";

    // Calling GetPidByName() to get the pid of Lab22
    err_status = GetPidByName(s_name, pid);

    // Note: There should not be a process called Lab22, therefore this should test your error message generation system.
    // if error status is equal to 0 then output the following (no error)
    if (err_status == 0) 
    {
        cout << "PID: " << pid <<  " Name: " << s_name << endl;
    }

    // otherwise do the following 
    else 
    {
        // get the err_status and set it to err_message
        err_message = GetErrorMsg(err_status);
        cout << "The name does not exist." << endl;
        cout << "The error is as follows: " <<err_message << endl;
    }
    return 0;
}