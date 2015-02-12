#pragma once
#include "headers.h"
#include "task_queue.h"
#include "user_task.h"

template<class ThreadPool>
class TaskPerformer{
    ThreadPool * owner;
    int timeout;
    int id;
  public:
    TaskPerformer(ThreadPool* owner_ = 0, int timeout_ = -1, int id_ = 0) : owner(owner_), timeout(timeout_), id(id_) {}
    TaskPerformer(TaskPerformer const & that) {
        owner = that.owner;
        timeout = that.timeout;
        id = that.id;
    }

    ~TaskPerformer() {}
    TaskPerformer & operator = (TaskPerformer & that) {
        owner = that.owner;
        timeout = that.timeout;
        id = that.id;
        return *this;
    }

    void operator()() {
        if (owner == NULL) {
            io_mutex.lock();
            std::cout << "no ThreadPool obj provided, TaskPerformer " << id << " exists\n";
            io_mutex.unlock();
            return;
        }

        io_mutex.lock();
        std::cout << "Starting TaskPerformer " << id << ", with timeout = " << timeout << "\n";
        io_mutex.unlock();
        while(true) {
            owner->inc_free_threads();
            std::pair<bool, UserTask* > task_info = owner->get_queue().get_task(timeout);
            owner->dec_free_threads();
            if (task_info.first) {
                task_info.second->run();
            }
            else {
                break;//this means that current thread is not hot and time is out - > exiting
            }
            if (owner->received_kill_signal()) break;
            boost::this_thread::yield();
        }
        io_mutex.lock();
        std::cout << "TaskPerformer " << id << " with timeout " << timeout << " stopped "  << "\n";
        io_mutex.unlock();
    }


};
