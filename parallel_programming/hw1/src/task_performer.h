#pragma once
#include "headers.h"

class TaskPerformer{
  int timeout;
  public:
    TaskPerformer(): timeout(-1) {}
    TaskPerformer(int timeout_) : timeout(timeout_) {}
    TaskPerformer(TaskPerformer const & that) {
        timeout = that.timeout;
    }

    ~TaskPerformer() {}
    TaskPerformer & operator = (TaskPerformer & that) {
        timeout = that.timeout;
        return *this;
    }

    void operator()() {
      while(true) {
        boost::this_thread::yield();
      }
    }


};
