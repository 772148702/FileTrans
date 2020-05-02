//
// Created by Shinelon on 2020/4/21.
//

#include <thread>
#include "fcntl.h"
#include "eventBase.h"


namespace  handy {

    EventImp::EventImp(EventBase *base,int taskCap):m_base(base),m_poller(CreatePoller()),m_exit(false),m_tasks(taskCap) {

    }
    void EventImp::Loop() {

        while(!m_exit) {
            LoopOnce(1000);
        }
    }
    void EventImp::LoopOnce(int waitMs) {
            m_poller->LoopOnce(waitMs);
    }

    void EventImp::SafeCall(Task &&task) {
         m_tasks.Push(move(task));
         WakeUp();
    }

    void EventImp::WakeUp() {
        int r = write(m_wakeupFds[1],"",1);
        fatalif(r <= 0, "write error wd %d %d %s", r, errno, strerror(errno));
    }

    void EventImp::Init() {
        int r = pipe(m_wakeupFds);
         fatalif(r,"pipe failed %d %s",errno,strerror(errno));
         r = util::addFdFlag(m_wakeupFds[0],FD_CLOEXEC);
         fatalif(r,"addFdFlag failed %d %s",errno,strerror(errno));
        r = util::addFdFlag(m_wakeupFds[1], FD_CLOEXEC);
        fatalif(r, "addFdFlag failed %d %s", errno, strerror(errno));
        trace("wakeup pipe created %d %d", m_wakeupFds[0], m_wakeupFds[1]);
        Channel *ch = new Channel(m_base, m_wakeupFds[0], kReadEvent);
        ch->SetReadCb([=]{
               char buf[1024];
               int  r=ch->Fd()>=0?::read(ch->Fd(),buf, sizeof(buf)):0;
               if(r>0) {
                   Task task;
                   while(m_tasks.PopWait(&task,0)) {
                       task();
                   }
               } else if(r==0) {
                   delete ch;
               } else if(errno=EINTR) {
               } else {
                   fatal("wakeup channel read error %d %d %s", r, errno, strerror(errno));
               }
        });
    }


    EventBase::EventBase(int taskCapacity) {
        m_EventImp.reset(new EventImp(this,taskCapacity));

    }

    EventBase::~EventBase() {}

    void EventBase::LoopOnce(int waitMs) {
            m_EventImp->LoopOnce(waitMs);
    }

    void EventBase::Loop() {
        m_EventImp->Loop();
    }

    EventBase &EventBase::Exit() {
       return  m_EventImp->Exit();
    }

    void EventBase::SafeCall(Task &&task) {
        m_EventImp->SafeCall(move(task));
    }

    void EventBase::WakeUp() {
        m_EventImp->WakeUp();
    }


    MultiBase &MultiBase::Exit() {
        for(auto &b:m_bases) {
            b.Exit();
        }
        return *this;
    }

    EventBase *MultiBase::AllocBase() {
        int c = m_id++;
        return &m_bases[c%m_bases.size()];
    }

    void MultiBase::Loop() {
        int sz = m_bases.size();
        std::vector<std::thread> ths(sz-1);
        for(int i=0;i<sz-1;i++) {
            std::thread t([this,i]{m_bases[i].Loop();});
            ths[i].swap(t);
        }
        m_bases.back().Loop();
        for(int i=0;i<sz-1;i++) {
            ths[i].join();
        }
    }
}