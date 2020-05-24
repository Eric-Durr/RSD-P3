//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//
//                     2º de grado de Ingeniería Informática
//
//              This class processes an FTP transactions.
//
//****************************************************************************

#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cerrno>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <iostream>
#include <dirent.h>

#include "common.h"

#include "ClientConnection.h"

ClientConnection::ClientConnection(int s, unsigned long addr)
{
    int sock = (int)(s);

    char buffer[MAX_BUFF];
    server_addr = addr;
    control_socket = s;

    // Check the Linux man pages to know what fdopen does.
    fd = fdopen(s, "a+");
    if (fd == NULL)
    {
        std::cout << "Connection closed" << std::endl;

        fclose(fd);
        close(control_socket);
        ok = false;
        return;
    }

    ok = true;
    data_socket = -1;
    quit = false;
};

ClientConnection::~ClientConnection()
{
    fclose(fd);
    close(control_socket);
}

int connect_TCP(uint32_t address, uint16_t port)
{
    // Implement your code to define a socket here
    struct sockaddr_in sin;
    int s;

    s = socket(AF_INET, SOCK_STREAM, 0); // TCP socket

    if (s < 0)
        errexit("Unable to create socket: %s\n", strerror(errno));

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = address; // Accept connection from address
    sin.sin_port = htons(port);    // Port number

    // if condition below checks if the socket, in this case "s", connects to de address specified by "sin"

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        errexit("Unable to connect to %s\n", address, strerror(errno));

    return s;
}

void ClientConnection::stop()
{
    close(data_socket);
    close(control_socket);
    quit = true;
}

#define COMMAND(cmd) strcmp(command, cmd) == 0

// This method processes the requests.
// Here you should implement the actions related to the FTP commands.
// See the example for the USER command.
// If you think that you have to add other commands feel free to do so. You
// are allowed to add auxiliary methods if necessary.

void ClientConnection::WaitForRequests()
{
    if (!ok)
    {
        return;
    }

    fprintf(fd, "220 Service ready\n");

    while (!quit)
    {

        fscanf(fd, "%s", command);

        /* The user identification is requiered by 
           the server to gain access to its file system.
           This command will normally be the first command 
           transmitted by the user after the control connections are made. 
        */
        if (COMMAND("USER"))
        {
            UserCommand(fd, arg);
        }

        /* This argument field is a string that spicfies the user´s password. 
           This command must be inmediatly proceeded by the user name command.
        */
        else if (COMMAND("PASS"))
        {
            PassCommand(fd, arg);
        }

        /* PWD. Print Working Directory, prints the directory, where the user is located */
        else if (COMMAND("PWD"))
        {
            PwdCommand(fd, arg);
        }

        /* CWD. Change working directory, This commands allows the 
           user to work with a different directory or dataset for file 
           storage or retrieval without altering his login or accounting 
           information
        */
        else if (COMMAND("CWD"))
        {
            CwdCommand(fd, arg);
        }

        /* The argument is a HOST-PORT specification for the data port to be used in data connection.
           If this command is used, the argument is the concatenation of a 32-bit internet host 
           address and a 16-bit TCP port address 
        */
        else if (COMMAND("PORT"))
        {
            PortCommand(fd, arg);
        }

        /* Passive mode switch command: This command requests the server-DTP to "listen" on a data port
           and to wait for a connection rather than initiate one upon receipt of a transfer command.
           The response to this command includes the host and port address this server is listening to
        */
        else if (COMMAND("PASV"))
        {
            PasvCommand(fd, arg);
        }

        /*  This command causes the server-DTP to accept the data transfered via the data connection and to store
            the data as a file at the server site 
        */
        else if (COMMAND("STOR"))
        {
            StorCommand(fd, arg);
        }

        /* This command causes the server-DTP to transfer a copy of the file, specified in
           the pathname, to the server- or user-DTP at the other end of the data connection.
        */
        else if (COMMAND("RETR"))
        {
            RetrCommand(fd, arg);
        }

        /* This command causes a list to be sent from the server to the passive DTP
           If the pathname specifies a directory or other group of file, the server should
           transfer a list of files in the specified directory. 
        */
        else if (COMMAND("LIST"))
        {
            ListCommand(fd, arg);
        }

        /* This command is used to find out the type of operating system at the server */
        else if (COMMAND("SYST"))
        {
            printf("SYST: acivated syst command \n");
            fprintf(fd, "215 UNIX Type: L8.\n");
        }

        /* The argument specifies the representation type as described in the Section
           on Data Represenation and Storage. Examples:

           A - ASCII
           E - EBCDIC
           I - Image
           L - <Byte size>
        */
        else if (COMMAND("TYPE"))
        {
            fscanf(fd, "%s", arg);
            fprintf(fd, "200 OK\n");
        }

        /* This command terminates a USER and if file transfer is not in progress, the server closes the control conenction. */
        else if (COMMAND("QUIT"))
        {
            fprintf(fd, "221 Service closing control connection. Logged out if appropriate.\n");
            close(data_socket);
            quit = true;
            break;
        }

        else
        {
            fprintf(fd, "502 Command not implemented.\n");
            fflush(fd);
            printf("Command : %s %s\n", command, arg);
            printf("Internal server error\n");
        }
    }

    fclose(fd);

    return;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////     Commands private methods      ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClientConnection::UserCommand(FILE *&fd, char arg[MAX_BUFF])
{
    fscanf(fd, "%s", arg);
    printf("USER: %s\n", arg);

    fprintf(fd, "331 User name ok, need password\n");
}

void ClientConnection::PassCommand(FILE *&fd, char arg[MAX_BUFF])
{
    fscanf(fd, "%s", arg);
    printf("PASS: %s\n", arg);

    if (strcmp(arg, "1234") == 0)
    {
        fprintf(fd, "230 User logged in\n");
    }
    else
    {
        fprintf(fd, "530 Not logged in.\n");
        quit = true;
    }
}

void ClientConnection::PwdCommand(FILE *&fd, char arg[MAX_BUFF])
{
    printf("PWD: show directory in client\n");
    char path[MAX_BUFF];

    if (getcwd(path, sizeof(path)) != NULL)
        fprintf(fd, "257 Working Directory: \"%s\" \n", path);
}

void ClientConnection::CwdCommand(FILE *&fd, char arg[MAX_BUFF])
{
    fscanf(fd, "%s", arg);
    printf("CWD: changing to %s\n", arg);

    char path[MAX_BUFF];

    if (getcwd(path, sizeof(path)) != NULL) // getcwd gets current path
    {
        strcat(path, "/"); // Prints path in the standard output
        strcat(path, arg);

        if (chdir(path) < 0) // Checks if directory can be changed
            fprintf(fd, "550 Fail, can´t change directory.\n");
        else
            fprintf(fd, "250 Success, directory changed.\n");
    }
}

void ClientConnection::PortCommand(FILE *&fd, char arg[MAX_BUFF])
{

    printf("PORT: activated port command\n");
    p_mode = false;

    unsigned int ip[4];
    unsigned int port[2];

    fscanf(fd, "%d, %d, %d, %d, %d, %d", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]);

    uint32_t ip_addr = ip[3] << 24 | ip[2] << 16 | ip[1] << 8 | ip[0];
    uint16_t port_v = port[0] << 8 | port[1];

    data_socket = connect_TCP(ip_addr, port_v);

    fprintf(fd, "200 Ok\n");
}

void ClientConnection::PasvCommand(FILE *&fd, char arg[MAX_BUFF])
{
    printf("PASV: activated passive command\n");
    p_mode = true;

    struct sockaddr_in sin, sa;
    socklen_t sa_len = sizeof(sa);
    int sFD;

    sFD = socket(AF_INET, SOCK_STREAM, 0);

    if (sFD < 0)
        errexit("Unable to create socket: %s\n", strerror(errno));

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = server_addr;
    sin.sin_port = 0;

    if (bind(sFD, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        errexit("Unable to bind socket: %s\n", strerror(errno));

    if (listen(sFD, 5) < 0)
        errexit("Unable to listen: %s\n", strerror(errno));

    getsockname(sFD, (struct sockaddr *)&sa, &sa_len);

    fprintf(fd, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).\n",
            (unsigned int)(server_addr & 0xff),
            (unsigned int)((server_addr >> 8) & 0xff),
            (unsigned int)((server_addr >> 16) & 0xff),
            (unsigned int)((server_addr >> 24) & 0xff),
            (unsigned int)(sa.sin_port & 0xff),
            (unsigned int)(sa.sin_port >> 8));

    data_socket = sFD;
}

void ClientConnection::StorCommand(FILE *&fd, char arg[MAX_BUFF])
{
    fscanf(fd, "%s", arg);
    printf("STOR: %s\n", arg);

    char Buffer[MAX_BUFF];
    int newFile;
    int aux;

    newFile = open(arg, O_RDWR | O_CREAT, S_IRWXU);

    fprintf(fd, "150 File ok, creating connection\n");
    fflush(fd);

    if (newFile < 0)
    {
        fprintf(fd, "450 Requested file action not taken.\n");
        close(data_socket);
    }
    else
    {
        fflush(fd);

        struct sockaddr_in sa;
        socklen_t sa_len = sizeof(sa);
        char buffer[MAX_BUFF];
        int n;

        if (p_mode)
            data_socket = accept(data_socket, (struct sockaddr *)&sa, &sa_len);

        do
        {
            aux = read(data_socket, Buffer, sizeof(Buffer));
            write(newFile, Buffer, aux);
        } while (aux > 0);

        fprintf(fd, "250 Requested action Ok and completed.\n");
        close(newFile);
        close(data_socket);
    }
}

void ClientConnection::RetrCommand(FILE *&fd, char arg[MAX_BUFF])
{
    fscanf(fd, "%s", arg);
    printf("RETR: Retrieving file %s\n", arg);

    FILE *file = fopen(arg, "rb");

    if (!file)
    {
        fprintf(fd, "450 Requested file action cancelled. File isn't available.\n");
        close(data_socket);
    }

    else
    {

        fprintf(fd, "150 File status okay; oppening conection.\n");

        struct sockaddr_in sa;
        socklen_t sa_len = sizeof(sa);
        char buffer[MAX_BUFF];
        int n;

        if (p_mode)
            data_socket = accept(data_socket, (struct sockaddr *)&sa, &sa_len);

        do
        {
            n = fread(buffer, sizeof(char), MAX_BUFF, file);
            send(data_socket, buffer, n, 0);

        } while (n == MAX_BUFF);

        fprintf(fd, "226 Closing data connection.\n");
        fclose(file);
        close(data_socket);
    }
}

void ClientConnection::ListCommand(FILE *&fd, char arg[MAX_BUFF])
{
    printf("(LIST): Listing current directory\n");
    fprintf(fd, "125 Data connection already open.\n");

    struct sockaddr_in sa;
    socklen_t sa_len = sizeof(sa);
    char buffer[MAX_BUFF];
    std::string list_str;

    FILE *file = popen("ls", "r");

    if (!file)
    {
        fprintf(fd, "450 Requested proccess stoped.\n");
        close(data_socket);
    }

    else
    {

        if (p_mode)
            data_socket = accept(data_socket, (struct sockaddr *)&sa, &sa_len);

        while (!feof(file))
            if (fgets(buffer, MAX_BUFF, file) != NULL)
                list_str.append(buffer);

        send(data_socket, list_str.c_str(), list_str.size(), 0);

        fprintf(fd, "250 Closing data connection.\n");
        pclose(file);
        close(data_socket);
    }
}
