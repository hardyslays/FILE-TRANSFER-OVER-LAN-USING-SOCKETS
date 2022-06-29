#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>
#include <netinet/in.h>
#include <fstream>
#include <arpa/inet.h>

#define PORT 9000

using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        cerr << "NO IP ADDRESS OF SERVER PROVIDED. TERMINATING THE RPOGRAM...\n";
        exit(1);
    }

    //INITIALIZE THE SOCKET
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        cerr << "SOCKET SETUP FAILED. TERMINATING THE PROGRAM...\n";
        exit(1);
    }

    //DETAILS OF SOCKET
    sockaddr_in serv_info;
    serv_info.sin_family = AF_INET;
    serv_info.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &(serv_info.sin_addr));

    //CONNECT TO THE SERVER SOCKET
    if(connect(sockfd, (sockaddr *)&serv_info, sizeof(serv_info)) != 0)
    {
        cerr << "ERROR CONNETING TO THE SOCKET. TERMINATING PROGRAM...\n";
        exit(1);
    }

    int choice;
    char file[256], name[256];

    do{
        system("clear");

        cout << "**************************************************\n";
        cout << "                HELLO DEAR USER\n";
        cout << "**************************************************\n";
        cout << "\n\tENTER THE ABSOLUTE FILE PATH YOU WANT TO SEND:    ";
        cin >> file;

        fstream f;
        f.open(file, ios::in | ios::out);
        if(!f)
        {
            cout << "\n\n    THE FILE PATH YOU ENTERED IS INCORRECT.";
            choice = -1;
            send(sockfd, &choice, sizeof(choice), 0);
        }
        else{
            choice = 1;
            
            cout << "\n\n    ENTER THE FILE NAME YOU WANT TO SEND TO SERVER:    ";
            cin >> name;
            send(sockfd, &choice, sizeof(choice), 0);
            send(sockfd, &name, sizeof(name), 0);

            recv(sockfd, &choice, sizeof(choice), 0);
            if(choice == 0){
                cout << "\n\n\n    SERVER DECLINED THE TRANSFER REQUEST.";
            }
            else{
                //FILE TRANSFER
            cout << "\nFILE TRANSFER.";
            }
        }
        cout << "\n  DO YOU WANT TO CONTINUE? (0/1)\n";
        cin >> choice;
        send(sockfd, &choice, sizeof(choice), 0);

        if (choice == 1) {
            cout << "\nCONTINUING THE FILE TRANSFER PROCESSES...\n\n\n";
            sleep(1);
        }
        else{
            cout << "THANK YOU FOR USING ME. CLOSING THE CLIENT PROGRAM.\n";
        }

    }while(choice == 1);









    return 0;
}
