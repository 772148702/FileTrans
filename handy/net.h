//
// Created by Shinelon on 2020/4/16.
//

#ifndef NET_NET_H
#define NET_NET_H
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>
#include "port_posix.h"
namespace handy {


    struct net {
            template <class T>
            static  T hton(T v) {
                return port::htobe(v);
            }
            template <class T>
            static T ntoh(T v) {
                return port::htobe(v);
            }
            static int setNonBlock(int fd,bool value=true);
            static int setReuseAddr(int fd,bool value=true);
            static int setReusePort(int fd,bool value=true);
            static int setNoDelay(int fd,bool value=true);
    };
}
#endif //NET_NET_H
