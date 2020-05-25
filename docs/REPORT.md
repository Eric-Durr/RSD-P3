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


<br>
<br>
<br>
<br>


<div id="id4">

## 4. Test cases
<br>
<br>

Once the program is compiled and excuted (instructions above), wa are able to use the ftp server, in which we can manage multiple commands such as:

* **USER**:

With this command we can connect to the server with our user, then we must type in our password.

```
ftp> user
(username) John
---> USER John
331 User name ok, need password
Password: 1234
---> PASS XXXX
230 User logged in
---> SYST
215 UNIX type: L8
Remote system type is UNIX
Using binary mode to transfer files
```

When we introduce the username, the program asks for our password (PASS command), in this case, it mut be "1234", if everything works correctly, a message will tell us that we are signed in.

Also it will show the system info (SYST command).


* **PWD**

With this command the user can see where he is located.

```
ftp> pwd
---> PWD
257 Working Directory: "path/to/directory"
```

* **CWD**

With this command the user is able to switch from his current directory to another one.

```
ftp> cd ..
---> CWD ..
250 Succes, directory changed.
```

* **PASV**

With this command the user is able to switch to passive mode.

```
ftp> pass
Passive mode on
```

* **STOR**

With this command the user is able to store/put a file on the server.

```
ftp> put README
local: README remote: README
---> TYPE 1
200 ok
ftp: setsockopt (ignored): Permission denied
---> PORT 127,0,0,1,231,80
200 ok
---> STOR README
150 File ok, creating connection
250 Requested action Ok and completed.
17 bytes sent in 0.00 secs (188.6541 kB/s)
```

* **RETR**

With this command the user is able to retrieve/get a file from the server.

```
ftp> get README
local: README remote: README
ftp: setsockopt (ignored): Permission denied
---> PORT 127,0,0,1,231,80
200 ok
---> RETR README
150 File okay, oppening connection
226 Closing data connection
17 bytes recieved in 0.00 secs (20.6487 kB/s)
```

* **LIST**

With this command the user is able to list the directory he desires.

```
ftp> ls
---> TYPE A
200 OK
ftp: setsockopt (ignored): Permission denied
---> PORT 127,0,0,1,231,80
200 ok
---> LIST
125 Data connection already open
File1.txt
File2.txt
250 Closing data connection
```

* **QUIT**

This command will close the connection with the server and quit the ftp programm.

```
ftp> quit
---> QUIT
221 Service closing control connection. Logged out if appropiate
```

Comands like **STOR**, **LIST** and **RETR**, can be excuted in passive mode, only if we use the **PASV** command right before. 

```
ftp> pass
Passive mode on

ftp> get README
local: README remote: README
---> TYPE I
200 OK
ftp: setsockopt (ignored): Permission denied
---> PASV 
227 Entering Passive mode (127,0,0,1,231,115)         <- Passive mode
---> RETR README
150 File okay, oppening connection
226 Closing data connection
17 bytes recieved in 0.00 secs (20.6487 kB/s)
```

If the passive mode is not activated, the commands mentioned before will be executed in "active mode".
 

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
