//
// Created by Shinelon on 2020/4/21.
//

#ifndef HANDY_EVENTBASE_H
#define HANDY_EVENTBASE_H

#include <functional>
#include <memory>
#include "util.h"
#include "poller.h"
#include "threads.h"

namespace handy {
    struct TcpConnection;
    typedef std::shared_ptr<handy::TcpConnection> TcpConnPtr;
    typedef std::function<void(const TcpConnPtr&)> TcpCallBack;
    struct EventBase;
    struct  EventImp;
    struct PollerBase;
    struct EventBases: private noncopyable {
        virtual  EventBase *AllocBase() =0;
    };

    struct EventBase: public EventBases {
        explicit EventBase(int taskCapacity=0);
        ~EventBase() override;
        EventBase &Exit();
        void LoopOnce(int waitMs);
        void Loop();
        void WakeUp();
        void SafeCall(Task&& task);
        void SafeCall(const Task& task){SafeCall(move(task));}
        EventBase* AllocBase() override {return this;}
    public:
        std::unique_ptr<EventImp> m_EventImp;
    };

    struct MultiBase :public EventBases {
        MultiBase(int sz):m_id(0),m_bases(sz){}
        virtual EventBase* AllocBase();
        void Loop();
        MultiBase &Exit();

    private:
        std::atomic<int> m_id;
        std::vector<EventBase> m_bases;
    };

    struct EventImp {
        EventImp(EventBase* base,int taskCap);
        void Init();
        void Loop();
        void LoopOnce(int );
        EventBase &Exit() {m_exit = true;return *m_base;}
        void SafeCall(Task && task);
        void WakeUp();

        int m_wakeupFds[2];
        EventBase *m_base;
        PollerBase* m_poller;
        std::atomic<bool> m_exit;
        SafeQueue<Task> m_tasks;
    };


}



#endif //HANDY_EVENTBASE_H
