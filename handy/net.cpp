//
// Created by Shinelon on 2020/4/16.
//

#include "net.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <string>
#include "logging.h"
#include "util.h"

using namespace std;
namespace handy {
    int net::setNonBlock(int fd, bool value) {
        int flags = fcntl(fd,F_GETFL,0);
        if(flags<0) {
            return errno;
        }
        if(value)  {
            return fcntl(fd,F_SETFL,flags|O_NONBLOCK);
        }
        return fcntl(fd,F_SETFD,flags&~O_NONBLOCK);
    }


    int net::setReuseAddr(int fd, bool value) {
            int flag = value;
            int len = sizeof flag;
            return setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&flag,len);
    }

    int net::setReusePort(int fd, bool value) {
        int flag = value;
        int len = sizeof flag;
        return setsockopt(fd,SOL_SOCKET,SO_REUSEPORT, &flag, len);
    }

}