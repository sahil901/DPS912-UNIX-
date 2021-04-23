// Created By: Sahil Patel (159-065-176)
// Source Code from: https://www.unix.com/programming/33263-unix-c-get-files-folder.html

// Header File(s)
#include <iostream> 
#include <string>
#include <fstream> 
#include <sys/types.h> // from source code 
#include <dirent.h> // from source code 
#include <unistd.h>
#include <ctype.h> 
#include <stdlib.h> 

// to not use std::
using namespace std;

// main code of the program
int main() 
{
    // variables for processName, processID and memoryUsed
    string procName;
    string procID;
    string used_mem;
    
    // gotten from provided website on the lab 1 (provided source code)
    struct dirent*de = NULL;
    DIR*d = NULL;

    // saving the proc path in the dir_proc variable
    const string dir_proc = "/proc";
    
    // file 
    ifstream file;

    // Used to open up the proc directory 
    d = opendir(dir_proc.c_str());
   
    // if the directory is null then do the following (gotten from source code)
    if (d == NULL)
    {
        // error message 
        perror("Directory couldn't be opened!");
        
        // exit application (unsuccessful)
        exit(1);
    }

    // gotten from provided source code
    while (de = readdir(d))
    {
        // if the directory is an process directory then do the following 
        if (*de->d_name == '.' || !isdigit(de->d_name[0]))  
        {
            // did this because did not work other way i had intended
            continue; 
        }

        // string(s) that contain the process diretory and status file
        string save;
        string nameOfDir(de->d_name);
        string proc_path = dir_proc + "/" + nameOfDir;
        string path_stat = proc_path + "/status";

        // assigning procID to the directory name 
        procID = nameOfDir;

        // open file
        file.open(path_stat.c_str());

        // loops through files and gets each line 
        while (getline(file, save))
        {
            // find line that has Name 
            int name = save.find("Name");

            // if name is not then do the following 
            if (name != -1)
            {
                // get string from character 6 - 10
                procName = save.substr(6, 10);
            }

            // find line that has VmRss
            int mem = save.find("VmRSS");

            // if mem is not then do the following 
            if (mem != -1)
            {
                // save string from first space to last space
                used_mem = save.substr(save.find_first_of(" "), save.find_last_of(" "));
            }
        }

        // if the memory used is equal to or greater than 10000kb then print out the following information 
        if (stoi(used_mem) >= 10000)
        {
            cout << "Process Name: " << procName << "   Process ID: " << procID << "   Memory Used: " << used_mem << endl;
        }

        // close the file
        file.close();

    }
    // close the directory 
    closedir(d);

    // exit application (successful)
    return 0;
}