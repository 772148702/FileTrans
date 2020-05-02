//
// Created by Shinelon on 2020/4/22.
//

#ifndef HANDY_POLLER_H
#define HANDY_POLLER_H

#include <sys/epoll.h>
#include <sys/poll.h>
#include "util.h"
#include "conn.h"
#include <atomic>

namespace  handy {

const int kMaxEvents = 2000;
const int kReadEvent = POLLIN;
const int kWriteEvent = POLLOUT;
struct Channel;
    struct PollerBase: private noncopyable {
        int64_t m_id;
        int m_lastActive;
        PollerBase():m_lastActive(-1){
            static std::atomic<int64_t> id(0);
            m_id = ++id;
        }
        virtual void AddChannel(Channel* channel)=0;
        virtual void RemoveChannel(Channel* ch) = 0;
        virtual void UpdateChannel(Channel* ch) = 0;
        virtual void LoopOnce(int waitMs) = 0;
        virtual ~PollerBase(){};
    };
    PollerBase* CreatePoller();
}



#endif //HANDY_POLLER_H
