#include <asm-generic/errno-base.h>
#include <cstring>
#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <sys/select.h>
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 

#define PORT 9000
#define MAX 10

using namespace std;

int main(int argc, char *argv[])
{
    int opt = true;

    int master_socket, addrlen, new_socket, clientsock[MAX], activity, i, valread, fd;
    int max_fd;
    char buf[256], name[256];

    fd_set readfs;

    for(int i = 0; i < MAX; i++)
    {
        clientsock[i] = 0;
    }

    if( (master_socket = socket(AF_INET, SOCK_STREAM, 0) ) == 0)
    {
        cerr << "SOCKET FAILED";
        exit(1);
    }

    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt)) < 0)
    {
        cerr << "SET SOCKOPT";
        exit(1);
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if(bind(master_socket, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        cerr << "BINDING FAILED.";
        exit(1);
    }

    cout << " LISTENING ON PORT " << PORT << endl;

    if(listen(master_socket, 3) < 0)
    {
        cerr << "CANNOT LISTEN FOR CONNECTIONS.";
        exit(1);
    }

    addrlen = sizeof(addr);

    cout << "WAITING FOR INCOMING CONNECTIONS...\n\n";

    while(true)
    {
        FD_ZERO(&readfs);

        FD_SET(master_socket, &readfs);
        max_fd = master_socket;

        for(int i = 0; i < MAX; i++)
        {
            fd = clientsock[i];

            if(fd > 0)
                FD_SET(fd, &readfs);

            if(fd > max_fd)
                max_fd = fd;
        }

        activity = select(max_fd + 1, &readfs, NULL, NULL, NULL);

        if((activity < 0) && (errno != EINTR))
        {
            cerr << "ERROR IN SELECTING ACTIVITY...";
        }


        //ADDING A NEW CLIENT CONNECTION REQUEST
        if(FD_ISSET(master_socket, &readfs))
        {
            if((new_socket = accept(master_socket, (sockaddr *)&addr, (socklen_t *)&addrlen)) < 0)
            {
                cerr << "ERROR ACCEPTING THE CONNECTIONS";
                exit(1);
            }


            cout << "\n\n\n******************************\n";
            cout <<  " CLIENT CONNECTED:\n";
            cout << "******************************\n";
            cout << "\tSOCKET FD: " << new_socket << endl;
            cout << "\tIP: " << inet_ntoa(addr.sin_addr) << endl;
            cout << "\tPORT: " << ntohs(addr.sin_port) << endl;

            for(i = 0; i < MAX; i++)
            {
                if(clientsock[i] == 0)
                {
                    clientsock[i] = new_socket;
                    cout << "CLIENT ADDED TO SOCKET LIST.\n";
                    break;
                }
            }
        }

        // I\O FOR ALREADY CONNECTED CLIENTS
        for(i = 0; i < MAX; i++)
        {
            fd = clientsock[i];

            if(FD_ISSET(fd, &readfs))
            {
                if(read(fd, buf, sizeof(buf)) == 0)
                {
                    getpeername(fd, (sockaddr*)&addr, (socklen_t*)&addrlen);

                    cout << "\n\n\n******************************\n";
                    cout << " CLIENT DISCONNECTED:\n";
                    cout << "******************************\n";
                    cout << "\tIP: " << inet_ntoa(addr.sin_addr) << endl;
                    cout << "\tPORT: " << ntohs(addr.sin_port) << endl;

                    close(fd);
                    clientsock[i] = 0;
                }
                else
                {
                    getpeername(fd, (sockaddr*)&addr, (socklen_t*)&addrlen);

                    cout << "\n\n\n*****************************************\n";
                    cout << " CLIENT HAS SENT A FILE TRANSFER REQUEST:\n";
                    cout << "*****************************************\n";
                    cout << endl;
                    cout << "\tCLIENT IP: " << inet_ntoa(addr.sin_addr) << endl;
                    cout << "\tPORT: " << ntohs(addr.sin_port) << endl;
                    memset(buf, 0, sizeof(buf));
                    memset(name, 0, sizeof(name));
                    recv(fd, buf, sizeof(buf), 0);
                    cout << "\tFILE NAME: " << buf << endl << endl;
                    strcpy(name, buf);

                    cout << "*****************************************\n";
                    cout << "\tDO YOU WANT TO ACCEPT THE REQUEST (0/1)?\t";
                    cin >> buf[0];
                    send(fd, &buf, sizeof(buf), 0);

                    if(buf[0] == '0')
                        cout << "\tDECLINED THE REQUEST.\n";
                    else
                    {
                        //FILE TRANSFER
                        int cnfrm;
                        fstream f;
                        f.open(name, ios::out);
                        if(!f)
                        {
                            cerr << "\tFILE NOT OPENED";
                            getchar();
                        }

                        cout << "\n\n*****************************************\n";
                        cout << " FILE TRANSFER STARTED" << endl;
                        cout << "*****************************************\n";

                        while(recv(fd, &cnfrm, sizeof(cnfrm), 0) && cnfrm != 0)
                        {
                            recv(fd, buf, sizeof(buf), 0);
                            cout << "\t" << buf << endl;
                            f.write(buf, cnfrm);
                            
                            cnfrm = 0;
                            send(fd, &cnfrm, sizeof(cnfrm), 0);
                        }
                        f.close();
                        cout << "\n\nFILE TRANSFER COMPLETE.\n";
                        cout << "*****************************************\n";
                    }
                }
            }
        }


    }

    return 0;
}