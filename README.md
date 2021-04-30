# FILE-TRANSFER-OVER-LAN-USING-SOCKETS
This is a server-client model for File transferring over sockets.
It is made using C++ and sockets in C++ (Lib: sys/socket.h, netdb.h and netinet).

This porgram is made for my 2nd year OS porject evaluation and is free to use.
> # THERE ATE TWO MODELS FOR THE PROJECT:
> ## 1.SINGLE CLIENT HANDLING MODEL: 
>   In this model, the server can handle only one model at a time. If a second client sends the connection request, it would get rejected by the server side. The server side and the client side are one-on-one designed.
>   
>## 2.MULTIPLE CLIENT HANDLING MODEL:
> In this model, the server-side can handle many clients at a time. More than one client can send connection requests and connect with server, however the transfer request is served on First come First served basis. Whenever a new client joins or disconnets, a message pops up in the server terminal with the details of the client. Currently, the number of clients has been imited to 10, but it can be increased further. 
> 
> ### The approx. speed we are getting from the model for file transfer is 10^6 cycles per second which is about 240 MBPS, but it would also depend on hardware configuration and the pysical network used.


## HAPPY CODING!!!
