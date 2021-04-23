// Created By: Sahil Patel (159-065-176)
// Source Code & Further info gathered from: https://mwatler.github.io/unx511/Labs/Lab3/etherCtrl.cpp and https://www.includehelp.com/c-programs/get-ip-address-in-linux.aspx and https://man7.org/linux/man-pages/man7/netdevice.7.html
// Also from instructions as it literally told me exactly what to do (Cases 2, 3, and 4)...

// Header File(s)
#include <errno.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <unistd.h>

#define NAME_SIZE 16

using namespace std;

int main()
{
    int fd;
    int ret;
    int selection;
    struct ifreq ifr;
    char if_name[NAME_SIZE];
    unsigned char* mac = NULL;

    cout << "Enter the interface name: ";
    cin >> if_name;

    size_t if_name_len = strlen(if_name);
    if (if_name_len < sizeof(ifr.ifr_name)) {
        memcpy(ifr.ifr_name, if_name, if_name_len);
        ifr.ifr_name[if_name_len] = 0;
    }
    else {
        cout << "Interface name is too long!" << endl;
        return -1;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        cout << strerror(errno);
        return -1;
    }

    system("clear");
    do {
        cout << "Choose from the following:" << endl;
        cout << "1. Hardware address" << endl;
        cout << "2. IP address" << endl;
        cout << "3. Network mask" << endl;
        cout << "4. Broadcast address" << endl;
        cout << "0. Exit" << endl << endl;
        cin >> selection;
        switch (selection) {
        case 1:
            ret = ioctl(fd, SIOCGIFHWADDR, &ifr);
            if (ret < 0) {
                cout << strerror(errno);
            }
            else if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER) {
                cout << "not an Ethernet interface" << endl;
            }
            else {
                mac = (unsigned char*)ifr.ifr_hwaddr.sa_data;
                printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }
            break;

            // CASES 2 3 4 were coded by me 

        // I CODED THIS
        case 2:
            // Get or set the address of the device 
            ret = ioctl(fd, SIOCGIFADDR, &ifr);

            // if ret is larger than 0 then output the error
            if (ret > 0)
            {
                cout << strerror(errno);
            }

            // otherwise find the ip and display it
            else
            {
                struct sockaddr_in* ip_addy = (struct sockaddr_in*)&ifr.ifr_addr;
                cout << "IP Address: " << inet_ntoa(ip_addy->sin_addr);
            }
            break;

            // I CODED THIS
        case 3:
            // Get or set the network mask for a device 
            ret = ioctl(fd, SIOCGIFNETMASK, &ifr);

            // if ret is larger than 0 then output the error
            if (ret > 0)
            {
                cout << strerror(errno);
            }

            // otherwise find the network mask and display it
            else
            {
                struct sockaddr_in* net_mask = (struct sockaddr_in*)&ifr.ifr_addr;
                cout << "Network Mask: " << inet_ntoa(net_mask->sin_addr);
            }
            break;

            // I CODED THIS
        case 4:
            // Get or set the broadcast address for a device
            ret = ioctl(fd, SIOCGIFBRDADDR, &ifr);

            // if ret is larger than 0 then output the error
            if (ret > 0)
            {
                cout << strerror(errno);
            }

            // otherwise find the broadcast address and display it
            else
            {
                struct sockaddr_in* broad_addy = (struct sockaddr_in*)&ifr.ifr_addr;
                cout << "Broadcast Address: " << inet_ntoa(broad_addy->sin_addr);
            }
            break;
        }
        if (selection != 0) {
            char key;
            cout << "Press any key to continue: ";
            cin >> key;
            system("clear");
        }
    } while (selection != 0);


    close(fd);
    return 0;
}