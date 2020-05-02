//
// Created by Shinelon on 2020/4/24.
//

#include <thread>
#include <assert.h>
#include "threads.h"

namespace  handy {


    ThreadPool::ThreadPool(int threads, int taskCapacity, bool start):m_tasks(taskCapacity),m_threads(threads) {
            if(start) {
                this->Start();
            }
    }
    ThreadPool::~ThreadPool() {
        assert(m_tasks.Exited());
        if(m_tasks.Size()) {
            fprintf(stderr, "%lu tasks not processed when thread pool exited\n", m_tasks.Size());
        }
    }

    void ThreadPool::Start() {
        for(auto & th:m_threads) {
            std::thread t([this]{
               while(!m_tasks.Exited()) {
                   Task task;
                   if(m_tasks.PopWait(&task)) {
                       task();
                   }
               }
            });
            th.swap(t);
        }
    }

    bool ThreadPool::AddTask(Task &&task) {
        return m_tasks.Push(move(task));
    }


    void ThreadPool::Join() {
        for(auto& t:m_threads) {
            t.join();
        }
    }
}

