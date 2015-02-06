#pragma once
#include "headers.h"

/* this class encapsulates task functionality*/
class UserTask {
  public:
    UserTask(int duration_): duration(duration_), time_left(duration_), is_stopped(false) {  }
    void task_step(int time) {
        //sleep(1);
        boost::this_thread::yield();
    }

    // method calls user task
    void run() {
        for (time_left = duration; time_left > 0; time_left --) {
            task_step(time_left);
            if (is_stopped) break;
        }
    }

    void stop() {
        is_stopped = true;
    }

  private:
    int duration;
    int time_left;
    bool is_stopped;
};
