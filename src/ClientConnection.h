#if !defined ClientConnection_H
#define ClientConnection_H

#include <pthread.h>

#include <cstdio>
#include <cstdint>

const int MAX_BUFF = 1000;

class ClientConnection
{
public:
    ClientConnection(int s, unsigned long addr);
    ~ClientConnection();

    void WaitForRequests();
    void stop();

private:
    bool ok; // This variable is a flag that avois that the
             // server listens if initialization errors occured.

    FILE *fd; // C file descriptor. We use it to buffer the
              // control connection of the socket and it allows to
              // manage it as a C file using fprintf, fscanf, etc.

    char command[MAX_BUFF]; // Buffer for saving the command.
    char arg[MAX_BUFF];     // Buffer for saving the arguments.

    int data_socket;    // Data socket descriptor;
    int control_socket; // Control socket descriptor;
    
    bool quit;
    bool p_mode;
    unsigned long server_addr;


    //Commands private functions

    void UserCommand(FILE* &fd, char arg[MAX_BUFF]);
    void PassCommand(FILE* &fd, char arg[MAX_BUFF]);
    void PwdCommand(FILE* &fd, char arg[MAX_BUFF]);
    void CwdCommand(FILE* &fd, char arg[MAX_BUFF]);
    void PortCommand(FILE* &fd, char arg[MAX_BUFF]);
    void PasvCommand(FILE* &fd, char arg[MAX_BUFF]);
    void StorCommand(FILE* &fd, char arg[MAX_BUFF]);
    void RetrCommand(FILE* &fd, char arg[MAX_BUFF]);
    void ListCommand(FILE* &fd, char arg[MAX_BUFF]);

};

#endif
