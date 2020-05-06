//
// Created by Shinelon on 2020/5/3.
//

#include "net.h"
#include "fileClient.h"
#include "iostream"
#include "file.h"

using namespace::handy;
FileClient::FileClient(EventBase *base, const std::string &host, unsigned short port,bool isGetInput) {
    m_pConn = TcpConnection::CreateConnection(base, host, port);
    m_pBase = base;
    if(isGetInput) {
        task = [&] {
            int LENUSERINPUT = 1024;
            char userinput[LENUSERINPUT];
            sleep(1);
            while(true) {
                printf("\t> ");
                std::cin>>userinput;
                info("input %s",userinput);
                //fgets(userinput, LENUSERINPUT, stdin);	// in order to give \
					a filename with spaces, put ':' \
					instead of ' '. If a command needs \
					x paths, and y (y > x) paths are \
					provided, y - x paths will be \
					ignored.
            }
        };
    }
}

void FileClient::GetInputStart() {
    task();
}

void FileClient::SendFile(char* path) {
     File file(path,"rb");
     Buffer buffer;
     int size;
     file.Read(buffer,size);
     file.Close();
     Buffer sendBuffer;
     EncodeMsg(Op::FilePut,buffer,sendBuffer,path);
     info("%s",sendBuffer.ToString().c_str());
     m_pConn->m_outbuffer.Absorb(sendBuffer);
     m_pConn->SendOutput();
}

