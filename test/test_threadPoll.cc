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




TEST(test::TestBase,ThreadPool)
{
  ThreadPool pool(2,5, false);
  int processed = 0;
  int *p = &processed;
  int added = 0;
  for(int i=0;i<10;i++) {
      added+=pool.AddTask([=]{
          printf("task %d processed\n",i);
          ++*p;
      });
  }
    pool.Start();

    pool.Exit();
    pool.Join();
    printf("pool tested\n");
    ThreadPool pool2(2);
    usleep(100 * 1000);
    processed = 0;
    added = 0;
    for (int i = 0; i < 10; i++) {
        added += pool2.AddTask([=] {
            printf("task %d processed\n", i);
            ++*p;
        });
    }
    usleep(100 * 1000);
    pool2.Exit();
    pool2.Join();
    ASSERT_EQ(added, processed);

}