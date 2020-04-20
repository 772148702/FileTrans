

#include "test_harness.h"
#include "handy/net.h"
#include "handy/logging.h"
#include "port_posix.h"
#include "conn.h"
using namespace handy;
using namespace handy::port;



//测试端口是否阻塞
TEST(test::TestBase,net)
{
    int fd = socket(AF_INET,SOCK_STREAM,0);
    net::setNonBlock(fd,true);
    net::setReusePort(fd,true);
    net::setReuseAddr(fd,true);
    Ip4Addr addr(8080);

    bind(fd,(struct sockaddr* )&addr.GetAddr(),sizeof(struct sockaddr));
    listen(fd,5);

    int pfd;
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    while(1)
    {

         pfd = accept(fd, (struct sockaddr *) &raddr, &rsz);
         Ip4Addr peerIp(raddr);
         if(pfd!=-1)
            info("%s",peerIp.ToString().c_str());
        if(pfd!=-1) {
            char buf[20] = "123你好";
            write(pfd,buf,sizeof(buf));
            info("write complete");
            break;
        }

    }

}