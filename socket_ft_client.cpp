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


    //RUN THE PROGRAM
    char file_path[256], buf[256];
    fstream file;
    int cnfrm = 1;

    while(1)
    {
        cout << "ENTER THE FILE PATH YOU WANT TO TRANSMIT: ";
        cin >> file_path;
        file.open(file_path, ios::in);
        if(!file)
        {
            cerr << "ERROR OPENING FILE. TERMINATING PROGRAM...\n";
            exit(1);
        }
        send(sockfd, &cnfrm, sizeof(cnfrm), 0);
        cout << "WAITING FOR THE SERVER SIDE TO ACCEPT THE FILE...\n";
        recv(sockfd, &cnfrm, sizeof(cnfrm), 0);
        if(cnfrm == 0)
        {
            cout << "SUCCESSFULLY CONNECTED TO SERVER. TRANSFERRING FILE NOW...\n";
            cnfrm = 1;
        }
        else
        {
            cerr << "CANNOT TRANSFER THE GIVEN FILE.\n";
            continue;
        }
        
        while (file.read(buf, sizeof(buf)))
        {
            cnfrm = 1;
            send(sockfd, &cnfrm, sizeof(cnfrm), 0);
            send(sockfd, buf, sizeof(buf), 0);
            recv(sockfd, &cnfrm, sizeof(cnfrm), 0);
        }
        cnfrm = 0;
        file.close();
        cout << "FILE SUCCESSFULLY SENT.\n";
        send(sockfd, &cnfrm, sizeof(cnfrm), 0);
        
        cout << "DO YOU WANT TO CONTINUE? (0/1): ";
        cin >> cnfrm;
        if(!cnfrm)break;
    }

    //CLOSE THE PROGRAM
    cout << "THANK YOU MR. CLIENT. :)\n";
    return 0;
}