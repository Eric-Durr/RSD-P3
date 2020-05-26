//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//
//                     2º de grado de Ingeniería Informática
//
//                        Main class of the FTP server
//
//****************************************************************************

#include <cerrno>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <pthread.h>

#include <list>

#include "common.h"
#include "FTPServer.h"
#include "ClientConnection.h"

int define_socket_TCP(int port)
{
    // Include the code for defining the socket.

    // initializing socket address struct named as "sin"
    struct sockaddr_in sin;

    // One integer for the socket FILE DECRIPTOR
    int s;

    s = socket(AF_INET, SOCK_STREAM, 0); //Creating the socket and saving the ID (must be "SOCK_STREAM" for FTP)

    // If the socket is created the ID must be higher than 0 so we check this
    if (s < 0)
        errexit("Unable to create socket: %s\n", strerror(errno)); // Access to common.h function

    memset(&sin, 0, sizeof(sin));     // Sets the first (sin size) bytes of the block memory pointed by sin to 0
    sin.sin_family = AF_INET;         // The family variable of the structure contains the net family for the socket
    sin.sin_addr.s_addr = INADDR_ANY; // The IP address is initialized as ANY value in order to pick the firs avalaible
    sin.sin_port = htons(port);       // Then the port is storaged in the address struct with h to ns in order to translate it to network byte type from host byte type

    // Then, bind will associate the socket FD to an address
    // the call must success in order to continue so we check if it returns
    // a negative integer that launches an error.
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        errexit("Unable to bind with the port: %s\n", strerror(errno));

    // The "listen" method listen for conection on a socket that is reffered with sFD
    // and marks it as a passive socket. This socket will be used to accept incoming
    // connection requests.
    //
    // If it fails it returns a negative number and triggers the conditional
    if (listen(s, 5) < 0) // Check if server has enough resources
        errexit("Listen fail: %s\n", strerror(errno));

    // We return the socket File Descriptor (we will use it to refer our socket)
    return s;
}

// This function is executed when the thread is executed
// and is used to create a "ClientConnection" object
// which runs the method that listens the requests.
void *run_client_connection(void *c)
{
    ClientConnection *connection = (ClientConnection *)c;
    connection->WaitForRequests();

    return NULL;
}

// CONSTRUCTOR:     provides a value for the "port" atribute when the object
//                  is created
FTPServer::FTPServer(int port)
{
    this->port = port;
}

// Stop server function:    executes the FTP socket functions to close the socket
//                          and free the resources attached to it.
void FTPServer::stop()
{
    close(msock);
    shutdown(msock, SHUT_RDWR);
}

// run:     Starts the server executing all the funtions related.
void FTPServer::run()
{
    // First  a socket kind structure is created and named as fsin
    struct sockaddr_in fsin;
    int ssock;
    // We store in alen the size of the socket address structure
    socklen_t alen = sizeof(fsin);
    msock = define_socket_TCP(port); // This function must be implemented by you:
                                     // As well as the function returns the socket
                                     // File Descriptor when the socket is defined
                                     // we store that FD in the atribute msock.

    // Endless loop
    while (1)
    {
        pthread_t thread;
        ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
        if (ssock < 0)
            errexit("Fallo en el accept: %s\n", strerror(errno));

        ClientConnection *connection = new ClientConnection(ssock, fsin.sin_addr.s_addr);

        // Here a thread is created in order to process multiple
        // requests simultaneously
        pthread_create(&thread, NULL, run_client_connection, (void *)connection);
    }
}
