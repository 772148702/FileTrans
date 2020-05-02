//
// Created by Shinelon on 2020/4/24.
//

#ifndef HANDY_THREADS_H
#define HANDY_THREADS_H

#include <mutex>
#include "util.h"
#include <list>
#include <condition_variable>
#include <atomic>
#include <thread>

namespace  handy {
    template <typename  T>
    struct SafeQueue:private std::mutex, private  noncopyable {
        static const int wait_infinite = std::numeric_limits<int>::max();
        SafeQueue(size_t capacity=0):m_capacity(capacity),m_exit(false){}
        bool Push(T &&v);
        T PopWait(int waitMs = wait_infinite);
        bool PopWait(T *v, int waitMs = wait_infinite);

        size_t Size();
        void Exit();
        bool Exited() {return m_exit;}
        private:
        std::list<T> m_items;
        std::condition_variable m_ready;
        size_t m_capacity;
        std::atomic<bool> m_exit;
        void WaitReady(std::unique_lock<std::mutex> &lk, int waitMs);
    };


    template<typename T>
    bool SafeQueue<T>::Push(T &&v) {
        std::lock_guard<std::mutex> lk(*this);
        if(m_exit||(m_capacity&&m_items.size()>=m_capacity)) {
            return false;
        }
        m_items.push_back(std::move(v));
        m_ready.notify_one();
        return true;
    }

    template<typename T>
    bool SafeQueue<T>::PopWait(T *v, int waitMs) {
        std::unique_lock<std::mutex> lk(*this);
        WaitReady(lk,waitMs);
        if(m_items.empty()) {
            return false;
        }
        *v = std::move(m_items.front());
        m_items.pop_front();
        return true;
    }

    template<typename T>
    void SafeQueue<T>::WaitReady(std::unique_lock<std::mutex> &lk, int waitMs) {
        if(m_exit||m_items.size()) {
            return;
        }
        if(waitMs==wait_infinite) {
            m_ready.wait(lk,[this]{return m_exit||!m_items.size();});
        } else  {
            auto tp = std::chrono::steady_clock::now() + std::chrono::milliseconds(waitMs);
            while(m_ready.wait_until(lk,tp)!=std::cv_status::timeout&&m_items.empty()&&!m_exit) {
            }
        }
    }

    template<typename T>
    size_t SafeQueue<T>::Size() {
        std::lock_guard<std::mutex> lk(*this);
        return m_items.size();
    }

    template<typename T>
    void SafeQueue<T>::Exit() {
        std::lock_guard<std::mutex> lk(*this);
        m_exit = true;
        m_ready.notify_all();
    }

    template<typename T>
    T SafeQueue<T>::PopWait(int waitMs) {
        std::unique_lock<std::mutex> lk(*this);
        WaitReady(lk,waitMs);
        if(!m_items.size()) {
            return T();
        }
        T r=std::move(m_items.front());
        m_items.pop_front();
        return r;
    }

    typedef std::function<void()> Task;

    struct ThreadPool:private noncopyable {
        ThreadPool(int threads,int taskCapacity=0,bool start = true);
        ~ThreadPool();
        void Start();
        ThreadPool &Exit() {
            m_tasks.Exit();
            return *this;
        }
        bool AddTask(Task && task);
        bool AddTask(Task &task) {return AddTask(Task(task));}
        size_t TaskSize() {return m_tasks.Size();}
        void Join();
    private:
        SafeQueue<Task> m_tasks;
        std::vector<std::thread> m_threads;
    };


}


#endif //HANDY_THREADS_H
