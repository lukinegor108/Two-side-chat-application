# Two-side-chat-application
Terrible made and terrible working programm for chating in terminal(first steps in c-sockets).
THIS APPLICATION IS NOT WELL MADE. YOU SHOULD NOT USE IT FOR YOUR CHATTING.
I've applied my knowledge from the "Hands-on with networking in C" by Lewis Van Winkle
I hope, that I'll post some well made chat applications later. 

Compilation:
The server and client are made for Unix-based, Windows and Mac operating systems. I tried to use the server on Ubuntu and clients on Ubuntu and Mac OS X, it was working. You can chek it on Windows and give some reply.

Usage:
(NOTICE that SERVER here is a second client, which is chatting with the CLIENT, I thought, that it would be good idea for two-side chatroom)
let SERVER, CLIENT be your compiled files. First run the SERVER(you have to choose port number inside of the code). Than you can run two CLIENT on a different device(or the same device):
  ./SERVER
  ./CLIENT2 [ip] [port]
  , where [ip] - IP address of the server, where the SERVER is running, and [port] - port, which you input inside of the SERVER programm
  If CLIENT was connected succefuly to the SERVER, than CLIENT can send a message, after that SERVER can reply. They can write only one message at the time(and less than 1000 bits). Than that cycle reapets until the SERVER or the CLIENT will stop.
