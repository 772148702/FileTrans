//
// Created by Shinelon on 2020/4/30.
//

#include <thread>
#include "test_harness.h"
#include "handy/net.h"
#include "handy/logging.h"

#include "threads.h"
using namespace handy;
using namespace handy::port;




TEST(test::TestBase,Server)
{
    SafeQueue<int> Sq;

    std::thread st1 ( [&] {
        for(int i=0;i<100;i++) {
          //  info("thread1 push %d",i);
            Sq.Push(std::move(i));
        }
       Sq.Exit();
    });

    std::thread st2([&]{
        while(1) {
            int tmp=-1;
           if( Sq.PopWait(&tmp,10)) {
               info("thread2 pop %d", tmp);
           } else {
               break;
           }
        }
    });

    std::thread st3([&](){
        while(1) {
            int tmp=-1;
            if(Sq.PopWait(&tmp,10)) {
                info("thread3 pop %d",tmp);
            } else  {
                break;
            }

        }
    });
    st1.join();
    st2.join();
    st3.join();
}