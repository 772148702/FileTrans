//
// Created by Shinelon on 2020/5/3.
//

#include "net.h"
#include "fileClient.h"
#include "iostream"
#include "file.h"
#include<limits>
using namespace::handy;

FileClient::FileClient(EventBase *base, const std::string &host, unsigned short port) {
    m_pConn = TcpConnection::CreateConnection(base, host, port);
    m_pBase = base;
    m_pConn->SetReadCb(HandleInput);
//    if(isGetInput) {
//        task = [&] {
//            int LENUSERINPUT = 1024;
//            char userinput[LENUSERINPUT];
//            sleep(1);
//            while(true) {
//                printf("\t> ");
//                std::cin>>userinput;
//                info("input %s",userinput);
//                //fgets(userinput, LENUSERINPUT, stdin);	// in order to give \
//					a filename with spaces, put ':' \
//					instead of ' '. If a command needs \
//					x paths, and y (y > x) paths are \
//					provided, y - x paths will be \
//					ignored.
//            }
//        };
//    }
}

void FileClient::GetInputStart() {
}

//path 必须是当前目录下的文件！
bool FileClient::SendFileReq(std::string path) {
     File file((char *)path.c_str(),"rb");
     if(!file.IsExist()) {
         FileNotExist(m_pConn,path);
         return false;
     }
     Buffer buffer;
     int size;
     file.Read(buffer,size);
     file.Close();
     Buffer sendBuffer;
     EncodeMsg(Op::FilePut,buffer,sendBuffer,(char*)path.c_str());
     info("%s",sendBuffer.ToString().c_str());
     m_pConn->m_outbuffer.Absorb(sendBuffer);
     m_pConn->SendOutput();
     return true;
}

bool FileClient::ReceFileReq(std::string path) {
   Buffer buffer;
   EncodeMsg(Op::FileGet,buffer,buffer,(char*)path.c_str());
    m_pConn->m_outbuffer.Absorb(buffer);
    m_pConn->SendOutput();
    return true;
}

bool FileClient::LsServerCurrentPath() {
    Buffer buffer;
    EncodeMsg(Op::FileLs,buffer,buffer);
    m_pConn->m_outbuffer.Absorb(buffer);
    m_pConn->SendOutput();
    return true;
}

/*
 * handle the message from server
 * the response of receive file
 * the response of massage
 */
 void FileClient::HandleInput(TcpConnPtr conn) {
    int size;
    Op op;
    std::string fileName;
    if(!DecodeMsg(conn->m_inbuffer,size,op,fileName) ) {
        return ;
    }
     std::string msgContent;
     std::string path;
    switch (op) {
        case Op::FileGet:
            path = fileName;
            ReceFileRes(conn,path,size);
            info("write file %s",path.c_str());
            break;
        case Op::Msg:
            conn->m_inbuffer.PopStringWithoutDataLength(msgContent,size);
            info("rece msg %s",msgContent.c_str());
            break;
        case Op::FileLs:
            conn->m_inbuffer.PopStringWithoutDataLength(msgContent,size);
            info("curDir is %s",msgContent.c_str());
            break;

        default:
            return;
    }
}

 void FileClient::ReceFileRes(TcpConnPtr conn,std::string path,int size) {
    File file((char *)path.c_str(),"wb");
    file.Write(conn->m_inbuffer,size);
    file.Close();
    conn->m_inbuffer.Consume(size);
}

bool FileClient::CdServerDir(std::string target) {
     Buffer temp;
     temp.Append(target.c_str(),target.size());
     EncodeMsg(Op::Cd,temp,m_pConn->m_outbuffer);
     m_pConn->SendOutput();
     return true;
}

bool FileClient::PwdServerDir() {
     Buffer temp;
    EncodeMsg(Op::Pwd,temp,m_pConn->m_outbuffer);
    m_pConn->SendOutput();
    return true;
}


bool InputCommanderClient::Init(EventBase *base, const std::string &host, unsigned short port) {
     t1=std::make_shared<std::thread>(std::thread([&](){
        fileClient = std::make_shared<FileClient>(base,host,port);
        base->Loop();
    }));
    return true;
}


void split(const std::string& s, std::vector<std::string>& tokens, const char& delim = ' ') {
    tokens.clear();
    size_t lastPos = s.find_first_not_of(delim, 0);
    size_t pos = s.find(delim, lastPos);
    while (lastPos != std::string::npos) {
        tokens.emplace_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delim, pos);
        pos = s.find(delim, lastPos);
    }
}

void InputCommanderClient::InputCommandToClient(EventBase *base, const std::string &host, unsigned short port) {
    t1=std::make_shared<std::thread>(std::thread([&](){
        fileClient = std::make_shared<FileClient>(base,host,port);
        base->Loop();
    }));
    int LENUSERINPUT = 1024;
    char userinput[LENUSERINPUT];

    handy::net::setNonBlock(0,false);
    std::vector<std::string> res;
    std::vector<std::string> command = {"lsServer","ls"};
    handy::net::setNonBlock(0, false);
    while(1) {
        printf("\t> ");
        sleep(1);
        int nbyte = 0;
//        std::cin>>userinput;
       while(1)  {
           nbyte=read(0,userinput,sizeof(userinput));
           if(nbyte>0) break;
       }
       userinput[nbyte-1]=0;

        std::string input=userinput;
      //  command.pop_back();
        res.clear();
        split(input,res,' ');
        std::cin.sync();
        memset(userinput,0,sizeof(userinput));
        if(nbyte<3) continue;
        if(res.size()>=3) {
            std::cout<<"wrong command"<<std::endl;
        }
        if (res[0]=="send" && res.size()==2) {
            if(fileClient->SendFileReq(res[1])) {
                info("send file %s",res[1].c_str());
            } else {
                info("file %s not exist",res[1].c_str());
            }
        } else if(res[0]=="receive" &&res.size()==2){
            if(fileClient->ReceFileReq(res[1])) {
                info("receive file %s request send",res[1].c_str());
            } else {
                info("receive file %s failed",res[1].c_str());
            }
        } else if(res[0]=="ls"&&res.size()==1) {
              std::string out;
              LsCurDir(out);
              info("the file in cur dir :%s",out.c_str());
        } else if(res[0]=="lsServer"&&res.size()==1){
              if(fileClient->LsServerCurrentPath()) {
                  info("send lsServer req");
              }
        } else if (res[0]=="cdServer"&&res.size()==2) {
            if(fileClient->CdServerDir(res[1])) {
                info("cd %s",res[1].c_str());
            }
        } else if(res[0]=="cd"&&res.size()==2) {
            if(ChangeDir(res[1])) {
                info("change dir successful!");
            } else {
                info("change failed, please check target dir!");
            }
        } else if(res[0]=="pwd"&&res.size()==1) {
            std::string out;
            PrintCurDir(out);
            info("curpath: %s",out.c_str());
        } else if(res[0]=="pwdServer"&&res.size()==1) {
            fileClient->PwdServerDir();
            info("send Server Dir req");
        } else if(res[0]=="exit") {
                return ;
        } else {
            info("wrong commands");
        }


//        for(int i=0;i<res.size();++i) {
//            std::cout<<res[i]<< " ";
//        }

        //fgets(userinput, LENUSERINPUT, stdin);	// in order to give \
					a filename with spaces, put ':' \
					instead of ' '. If a command needs \
					x paths, and y (y > x) paths are \
					provided, y - x paths will be \
					ignored.

        //info("input %s",userinput);

    }
        t1->join();
}


