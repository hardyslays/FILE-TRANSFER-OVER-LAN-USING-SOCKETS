#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>
#include <netinet/in.h>
#include <fstream>

#define PORT 9000

using namespace std;

int main(int argc, char *argv[])
{
    int check; 
    //SETTING UP THE SOCKET
    int servfd = socket(AF_INET, SOCK_STREAM, 0);
    if(servfd <= 0)
    {
        cerr << "ERROR SETTING UP THE SOCKET. TERMINATING THE PROGRAM.\n";
        exit(1);
    }

    //SETTING UP DATA REQUIRED FOR THE SOCKET
    sockaddr_in serv_addr, cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //SETTING UP SOCKET OPTION FOR IMMEDIATE REBINDING
    int opt = 1;
    if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //BINDING THE SERVER TO THE SOCKET
    check = bind(servfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
    if(check < 0)
    {
        cerr << "BINDING FAILED. TERMINATING THE PROGRAM...\n";
        exit(1);
    }

    LISTEN:

    //LISTEN TO INCOMING CONNECTIONS
    check = listen(servfd, 1);
    if(check < 0 )
    {
        cerr << "ERROR LISTENING TO INCOMING CONNECTIONS. TERMINATING THE PROGRAM...\n";
        exit(1);
    }

    //ACCEPTING THE CONNECTIONS
    int clientfd = accept(servfd, (sockaddr *)&cli_addr, &clilen);
    cout << "\nCONNECTED TO THE CLIENT\n\n";
    sleep(1);

    int choice;
    char name[256], dir[] = "./downloads";

    CLIENT:
    do{
        system("clear");
        cout << "**************************************************\n";
        cout << "                  SERVER SIDE\n";
        cout << "**************************************************\n";

        cout << "\n\n    WAITING FOR CLIENT SIDE TO SEND FILE TRANSFER REQUEST.....\n\n";
        recv(clientfd, &choice, sizeof(choice), 0);
    }while(choice == -1);
    
    if(choice == 1)
    {

        system("clear");
        cout << "**************************************************\n";
        cout << "                  SERVER SIDE\n";
        cout << "**************************************************\n";
        recv(clientfd, &name, sizeof(name), 0);
        cout << "\n  THE CLIENT WANTS TO SEND A FILE.";
        cout << "\n      FILE NAME:    " << name;

        cout << "\n\n  DO YOU WISH TO CONTINUE? (0/1)\n";
        cin >> choice;

        if(choice == 0)
        {
            cout << "\n\n    DECLINED THE TRANSFER REQUEST.\n";
        send(clientfd, &choice, sizeof(choice), 0);
        }
        else {
            //FILE TRANSFER
            cout << "\nFILE TRANSFER.\n";
        send(clientfd, &choice, sizeof(choice), 0);
        }

        recv(clientfd, &choice, sizeof(choice), 0);
    }
    if(choice == 0)
    {
        cout << "\n\n  CLIENT PROGRAMMED CLOSED.\n  TERMINATING ITS SOCKET.";
        close(clientfd);

        cout << "\n\n  DO YOU WISH TO CONTINUE LISTENING TO OTHER REQUESTS? (0/1)\n";
        cin >> choice;
        if(choice == 0)
        {
            cout << "\n\n    OK!!! CLOSING THE MAIN SOCKET AND TERMINATING THE PROGRAM.\n";
            close(servfd);
        }
        else {
            cout << "\n\n    SETTING UP PROGRAM FOR ANOTHER CONNECTION REQUEST";
            sleep(1);
            goto LISTEN;
        }
    }
    else {

        
        goto CLIENT;
    }









    return 0;
}