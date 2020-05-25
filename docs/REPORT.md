# Report 3 | Socket Programming (Transport Layer)

<br>

Redes y Sistemas Distribuidos - ESIT (Escuela superior de Ingeniería y Tecnología) ULL

<br>
<br>
<br>
<br>
<br>
<br>
<br>

![socket front image](images/Portada.jpg)

<br>
<br>
<br>
<br>
<br>
<br>
<br>

    Report made by Éric Dürr Sierra and Noah Sánchez
                   (alu0101027005)     (alu0101134956)
<br>
<br>
<br>
<br>

<!-- end of cover page --->
***

The following document contains the sections which will introduce the different stages of the assignment of *socket programming* ( 3rd assignment ). All you will see
along this report will reffer to concepts of the fifth TCP/IP network layer ( Transport ).

This document will also include evidences of the teamwork ( as done in previous reports ) and a list of the system functions used for this assignment code.

<br>
<br>
<br>


## **Index** &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;Page:
 

## 1. [Description of developed aplications](#id1)
## 2. [Description of developed protocol](#id2)
## 3. [Guide for compilation and execution steps](#id3)
## 4. [Test cases](#id4)
## 5. [Appendix: Source code](#id5)


<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>

***

<!-- end of index page --->
<br>
<br>
<br>
<br>
 
<div id="id1">

## 1. Description of developed aplications.
<br>
<br>


<br>
<br>
<br>
<br>

<div id="id2">

## 2. Description of developed protocol.
<br>
<br>


<br>
<br>
<br>
<br>


<div id="id3">

## 3. Guide for compilation and execution steps
<br>
<br>

1. Compiling the server aplication (which is the one we programmed) is as easy as executing the order `make` in a linux terminal command line. This will give an executable of the server named as ***ftp_server***. We must be located in the ***src*** subdirectory of the project repossitory to do this.

if everything ran succesfully the terminal output should look like this:
```bash
~$ make 
   g++ -g -std=gnu++0x  ClientConnection.cpp FTPServer.cpp ftp_server.cpp 
   -o ftp_server -lpthread

```

2. Once the program was compiled we can leave the server running. To do this we must run the command `./ftp_server` leaving our server working on the background and waiting for a client to connect.

3. To open a client we execute the `ftp` command in a diferent linux terminal, once done we will enter to the ftp command line interface.

4. We must connect our client to the server entering the `open` command in order to open the conection, as arguments we must provide the IP address which is `localhost` and the port `2121` in our case. If all the steps succeeded the command line should look like this:

```bash
ftp> open localhost 2121
Connected to localhost.
220 Service ready
Name (localhost:user): 
```
5. As shown by the terminal the next step is to introduce our username and then our password. If all was right the console should look lie following:

```bash
331 User name ok, need password
Password:
230 User logged in
Remote system type is UNIX.
Using binary mode to transfer files.
ftp>
```
6. From here on we will be able to interact with the server through the ftp commands in the server, such as getting files, putting files, showing the current directory, etc.

<br>
<br>
<br>
<br>


<div id="id4">

## 4. Test cases
<br>
<br>


<br>
<br>
<br>
<br>


<div id="id5">

## 5. Appendix: Source code.
<br>
<br>


<br>
<br>
<br>
<br>
