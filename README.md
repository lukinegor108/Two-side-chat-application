# Two-side-chat-application

Asynchronous chatroom for two pepople only.

Compilation:
The server and client are made for Unix-based, Windows and Mac operating systems. I tried to use the server on Ubuntu and clients on Ubuntu and Mac OS X, it was working. You can chek it on Windows and give some reply.

gcc -o client client.c -lpthread  
gcc -o server server.c -lpthread

Usage:
(NOTICE that SERVER here is a second client, which is chatting with the CLIENT, I thought, that it would be good idea for two-side chatroom)
let SERVER, CLIENT be your compiled files. First run the SERVER(you have to choose port number inside of the code). Than you can run two CLIENT on a different device(or the same device):
  ./SERVER
  ./CLIENT2 [ip] [port]
  , where [ip] - IP address of the server, where the SERVER is running, and [port] - port, which you input inside of the SERVER programm
  If CLIENT was connected succefuly to the SERVER, than you can start messaging.
