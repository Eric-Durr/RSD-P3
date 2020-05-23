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

ClientConnection::ClientConnection(int s)
{
    int sock = (int)(s);

    char buffer[MAX_BUFF];

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
            fprintf(fd, "331 User name ok, need password\n");
        }

        else if (COMMAND("PASS"))
        {
            fscanf(fd, "%s", arg);
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
            printf("(PWD): SHOW\n");

            char path[MAX_BUFF];

            if (getcwd(path, sizeof(path)) != NULL)
                fprintf(fd, "257 Working Directory: \"%s\" \n", path);
        }
        else if (COMMAND("CWD"))
        {
            fscanf(fd, "%s", command);
            printf("(CWD):%s\n", arg);

            char path[MAX_BUFF];

            if (getcwd(path, sizeof(path)) != NULL) // getcwd gets current path
            {
                strcat(path, "/"); // Prints path in the standard output
                strcat(path, arg);

                if (chdir(path) < 0) // Checks if directory can be changed
                    fprintf(fd, "550, failed to change directory.\n");
                else
                    fprintf(fd, "250, changed directoy succesfully.\n");
            }
        }
        else if (COMMAND("PORT"))
        {
            // To be implemented by students
        }
        else if (COMMAND("PASV"))
        {
            // To be implemented by students
        }
        else if (COMMAND("STOR"))
        {
            // To be implemented by students
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
            printf("Comando : %s %s\n", command, arg);
            printf("Error interno del servidor\n");
        }
    }

    fclose(fd);

    return;
};
