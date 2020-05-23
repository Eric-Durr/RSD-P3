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

        if (COMMAND("USER"))
        {
            fscanf(fd, "%s", arg);
            printf("USER: %s\n", arg);

            fprintf(fd, "331 User name ok, need password\n");
        }

        else if (COMMAND("PASS"))
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
        else if (COMMAND("PWD"))
        {
            printf("PWD: show directory in client\n");
            char path[MAX_BUFF];

            if (getcwd(path, sizeof(path)) != NULL)
                fprintf(fd, "257 Working Directory: \"%s\" \n", path);
        }
        else if (COMMAND("CWD"))
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

        else if (COMMAND("PORT"))
        {

            printf("PASV: activated port command\n");
            p_mode = false;

            unsigned int ip[4];
            unsigned int port[2];

            fscanf(fd, "%d, %d, %d, %d, %d, %d", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]);

            uint32_t ip_addr = ip[3] << 24 | ip[2] << 16 | ip[1] << 8 | ip[0];
            uint16_t port_v = port[0] << 8 | port[1];

            data_socket = connect_TCP(ip_addr, port_v);

            fprintf(fd, "200 Ok\n");
        }

        /*Passive mode switch command: */
        else if (COMMAND("PASV"))
        {
            printf("PASV: activated passive command\n");
            p_mode = true;

            struct sockaddr_in sin, sa;
            socklen_t sa_len = sizeof(sa);
            int sFD;

            if ((sFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                errexit("Unable to create socket: %s\n", strerror(errno));

            memset(&sin, 0, sizeof(sin));
            sin.sin_family = AF_INET;
            sin.sin_addr.s_addr = server_addr;
            sin.sin_port = 0;

            if (bind(sFD, (struct sockaddr *) &sin, sizeof(sin)) < 0)
                errexit("Unable to bind socket with port: %s.\n", strerror(errno));

            if (listen(sFD, 5) < 0)
                errexit("Unable to listen: %s\n", strerror(errno));

            getsockname(sFD, (struct sockaddr *) &sa, &sa_len);

            fprintf(fd, "277 changed to Passive Mode");

            data_socket = sFD;

        }
        else if (COMMAND("STOR"))
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
                fprintf(fd, "450 Requested action not taken.\n");
            }
            else
            {
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
        else if (COMMAND("RETR"))
        {
            // To be implemented by students
        }
        else if (COMMAND("LIST"))
        {
            // To be implemented by students
        }
        else if (COMMAND("SYST"))
        {
            fprintf(fd, "215 UNIX Type: L8.\n");
        }

        else if (COMMAND("TYPE"))
        {
            fscanf(fd, "%s", arg);
            fprintf(fd, "200 OK\n");
        }

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
