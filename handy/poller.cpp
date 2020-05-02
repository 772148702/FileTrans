//
// Created by Shinelon on 2020/4/22.
//

#include <set>
#include "poller.h"



namespace handy {
#ifdef OS_LINUX
    struct PollerEpoll:public PollerBase {
        int m_fd;
        std::set<Channel*> m_liveChannels;
        struct epoll_event m_activeEvents[kMaxEvents];
        PollerEpoll();
        ~PollerEpoll();
        void AddChannel(Channel* ch) override;
        void RemoveChannel(Channel* ch) override;
        void UpdateChannel(Channel* ch) override;
        void LoopOnce(int waitMs) override;
    };

    PollerBase * CreatePoller() {
        return new PollerEpoll();
    }

    PollerEpoll::PollerEpoll() {
        m_fd = epoll_create(10);
        fatalif(m_fd < 0, "epoll_create error %d %s", errno, strerror(errno));
        info("poller epoll %d created", m_fd);
    }

    PollerEpoll::~PollerEpoll() {

    }

    void PollerEpoll::LoopOnce(int waitMs) {
            int64_t ticks = util::timeMill();
            m_lastActive = epoll_wait(m_fd,m_activeEvents,kMaxEvents,-1);
            int64_t  used = util::timeMill()- ticks;
            trace("epoll wait %d return %d errno %d used %lld millsecond", waitMs, m_lastActive, errno, (long long) used);
            fatalif(m_lastActive == -1 && errno != EINTR, "epoll return error %d %s", errno, strerror(errno));
            //info("1");
            if(m_lastActive==0) return;

            while(--m_lastActive>=0) {
                int i= m_lastActive;
                Channel * ch=(Channel* ) m_activeEvents[i].data.ptr;
                int events = m_activeEvents[i].events;
                if(ch) {
                    if(events &(kReadEvent|EPOLLERR)) {
                        trace("channel %lld fd %d handle read", (long long) ch->m_id, ch->m_fd);
                        ch->HandleRead();
                    } else if(events & kWriteEvent) {
                        trace("channel %lld fd %d handle write", (long long)  ch->m_id, ch->m_fd);
                        ch->HandleWrite();
                    } else {
                        fatal("unexpected poller events");
                    }
                }
            }
    }

    void PollerEpoll::AddChannel(Channel *ch) {
            struct epoll_event ev;
            memset(&ev,0,sizeof(ev));
            ev.events = ch->m_event|EPOLLET;
            ev.data.ptr = ch;
            trace("adding channel %lld fd %d events %d epoll %d", (long long) ch->m_id, ch->m_fd, ev.events, m_fd);
            int r = epoll_ctl(m_fd, EPOLL_CTL_ADD, ch->m_fd, &ev);
            fatalif(r, "epoll_ctl add failed %d %s", errno, strerror(errno));
            m_liveChannels.insert(ch);
    }

    void PollerEpoll::UpdateChannel(Channel *ch) {
        struct epoll_event ev;
        memset(&ev,0,sizeof(ev));
        ev.events = ch->m_event;
        ev.data.ptr = ch;
        trace("modifying channel %lld fd %d events read %d write %d epoll %d", (long long) ch->m_id, ch->m_fd, ev.events & EPOLLIN, ev.events & EPOLLOUT, m_fd);
        int r = epoll_ctl(m_fd, EPOLL_CTL_MOD, ch->m_fd, &ev);
        fatalif(r, "epoll_ctl mod failed %d %s", errno, strerror(errno));
    }

    void PollerEpoll::RemoveChannel(Channel *ch) {
        trace("deleting channel %lld fd %d epoll %d", (long long) ch->m_id, ch->m_fd, m_fd);
        m_liveChannels.erase(ch);
        for (int i=m_lastActive;i>=0;i--) {
            if(ch==m_activeEvents[i].data.ptr) {
                m_activeEvents[i].data.ptr = nullptr;
                break;
            }
        }
    }

#endif
}