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

using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        cerr << "NO IP ADDRESS PROVIDED.\n";
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == 0)
    {
        cerr << "ERROR OPENING SOCKET\n";
        exit(1);
    }

    //DETAILS OF SOCKET
    sockaddr_in serv_info;
    serv_info.sin_family = AF_INET;
    serv_info.sin_port = htons(PORT);
    inet_pton(AF_INET, (char *)argv[1], &(serv_info.sin_addr));

    //CONNECT TO THE SERVER SOCKET
    if(connect(sockfd, (sockaddr *)&serv_info, sizeof(serv_info)) != 0)
    {
        cerr << "ERROR CONNETING TO THE SOCKET. TERMINATING PROGRAM...\n";
        exit(1);
    }

    char buf[256];
    char file[256], name[256];

    do
    {
        memset(buf, ' ', sizeof(buf));
        cout << endl;
        system("clear");
        
        cout << "**************************************************\n";
        cout << "                HELLO DEAR USER\n";
        cout << "**************************************************\n";
        cout << "\n    ENTER THE ABSOLUTE FILE PATH YOU WANT TO SEND:    ";
        cin >> file;

        fstream f;
        f.open(file, ios::in | ios::out);
        if(!f)
        {
            cout << "\n    THE FILE PATH YOU ENTERED IS INCORRECT.";
            buf[0] = '0';
        }
        else{
            buf[0] = '1';

            cout << "\n    ENTER THE FILE NAME YOU WANT TO SEND TO SERVER:    ";
            cin >> name;
            send(sockfd, &buf, sizeof(buf), 0);
            send(sockfd, &name, sizeof(name), 0);

            cout << "\n\n    WAITING FOR SERVER SIDE TO ACCEPT THE REQUEST...";

            recv(sockfd, &buf, sizeof(buf), 0);
            if(buf[0] == '0'){
                cout << "\n\n    SERVER DECLINED THE TRANSFER REQUEST.";
            }
            else{
                //FILE TRANSFER
                cout << "\nFILE TRANSFER STARTED...";
                int cnfrm;
                char d;

                while ( !f.eof())
                {
                    memset(buf, 0, sizeof(buf));
                    f.read(buf, sizeof(buf));
                    cnfrm = f.gcount();
                    send(sockfd, &cnfrm, sizeof(cnfrm), 0);
                    send(sockfd, buf, sizeof(buf), 0);

                    recv(sockfd, &cnfrm, sizeof(cnfrm), 0);
                }
                cnfrm = 0;
                send(sockfd, &cnfrm, sizeof(cnfrm), 0);

                f.close();
                cout << "TRANSFER DONE\n";
            }
        }
        
        cout << "\n\n  DO YOU WANT TO CONTINUE? (0/1)\n";
        cin >> buf[0];

        if (buf[0] == '1') {
            cout << "CONNTINUING...";
        }
        else{
            cout << "THANK YOU FOR USING ME. CLOSING THE CLIENT PROGRAM.\n";
        }
    }while(buf[0] == '1');

    close(sockfd);

    return 0;
}
