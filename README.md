# FileTrans

该项目是出于练手目的防handy库，写的一个简易的handy库。并利用这个handy库写的文件传输的服务端与客户端。由于并未采用FTP的协议（这是自定义的协议），故无法与FTP服务器直接对接。本项目还是一个toy项目，主要是学习多线程preactor模型，以及应用层的怎么封装信息使用的。

## Usage:

bin path

1. server: \bin\server\servermain
2. client:\bin\client\clientmain

command:

1. print client directory files:  ls 
2. print server directory files: lsServer
3. print current working directory: pwd
4. ptrint server current working directory: pwdServer
5. send file to server: send filename
6. recevice file from server: receive filename
7. change dir in client: cd dirname
8. change dir in server: cdServer dirname
9. exit client: exit





