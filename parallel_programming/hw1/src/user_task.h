#pragma once
#include "headers.h"

/* this class encapsulates task functionality*/
class UserTask {
  public:
    UserTask(int duration_ = 0): duration(duration_), time_left(duration_), is_stopped(false) {  }
    UserTask(UserTask const & task): duration(task.duration), time_left(task.time_left), is_stopped(task.is_stopped) {  }
    UserTask& operator = (UserTask const& task) {
      duration = task.duration;
      time_left = task.time_left;
      is_stopped = task.is_stopped;
      return *this;
    }

    void task_step(int time) {
        //sleep(1);
        std::cout << "User task is processed:  "<< time << "\n";
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
      std::cout << "stop task called\n";
        is_stopped = true;
    }

  private:
    int duration;
    int time_left;
    bool is_stopped;
};
