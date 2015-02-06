#pragma once
#include "headers.h"
#include "task_queue.h"
#include "user_task.h"

template<class ThreadPool>
class TaskPerformer{
    ThreadPool * owner;
    int timeout;
  public:
    TaskPerformer(): owner(NULL), timeout(-1) {}
    TaskPerformer(ThreadPool* owner = NULL, int timeout_ = -1) : timeout(timeout_) {}
    TaskPerformer(TaskPerformer const & that) {
        owner = that.owner;
        timeout = that.timeout;
    }

    ~TaskPerformer() {}
    TaskPerformer & operator = (TaskPerformer & that) {
        owner = that.owner;
        timeout = that.timeout;
        return *this;
    }

    void run_task() {
      if(owner == NULL) {
        std::cout << "no ThreadPool obj provided, TaskPerformer exists\n";
        return;
      }
      std::cout << "starting TaskPerformer, with timeout = " << timeout << "\n";
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
        boost::this_thread::yield();
      }
    }


};
