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
    if(servfd < 0)
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

    //BINDING THE SERVER TO THE SOCKET
    check = bind(servfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
    if(check < 0)
    {
        cerr << "BINDING FAILED. TERMINATING THE PROGRAM...\n";
        exit(1);
    }

    //LISTEN TO INCOMING CONNECTIONS
    check = listen(servfd, 2);
    if(check < 0 )
    {
        cerr << "ERROR LISTENING TO INCOMING CONNECTIONS. TERMINATING THE PROGRAM...\n";
        exit(1);
    }

    //ACCEPTING THE CONNECTIONS
    int clientfd = accept(servfd, (sockaddr *)&cli_addr, &clilen);

    //RUN THE PROGRAM
    char file_dest[256], buf[256];
    fstream file;
    int cnfrm = 0;
    while(1)
    {
        recv(clientfd, &cnfrm, sizeof(cnfrm), 0);
        if(cnfrm == 1)
        {
            cout << "SUCCESFULLY CONNECTED TO THE CLIENT.\n";
            cnfrm = 0;
        }
        cout << "ENTER THE FILE DESTINATION: ";
        cin >> file_dest;
        file.open(file_dest, ios::out);
        if(!file)
        {
            cerr << "ERROR ACCESSING FILE. CANNOT TRANSFER THE FILE.\n";
            cnfrm = 1;
            send(clientfd, &cnfrm, sizeof(cnfrm), 0);
            continue;
        }
        send(clientfd, &cnfrm, sizeof(cnfrm), 0);
        cout << "RECIEVING THE FILE FROM CLIENT...\n";

        while(recv(clientfd, &cnfrm, sizeof(cnfrm), 0) && cnfrm == 1)
        {
            cnfrm = 0;
            recv(clientfd, buf, sizeof(buf), 0);
            cout << buf << endl;
            file.write(buf, sizeof(buf));
            send(clientfd, &cnfrm, sizeof(cnfrm), 0);
        }
        if(cnfrm == 0)
        {
            file.close();
            cout << "FILE RECIEVED SUCCESSFULLY.\n";
        }
        
        cout << "DO YOU WANT TO CONTINUE? (0/1): ";
        cin >> cnfrm;
        if(!cnfrm)break;
    }

    //CLOSE THE PROGRAM
    cout << "THANKYOU FOR USING ME. :)\n";
    return 0;
}