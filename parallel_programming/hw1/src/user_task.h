#pragma once
#include "headers.h"

/* this class encapsulates task functionality*/
class UserTask {
  public:
    UserTask(int task_id_ = 0, int duration_ = 0): task_id(task_id_), duration(duration_), time_left(duration_), is_stopped(true) {  }
    UserTask(UserTask const & task): task_id(task.task_id), duration(task.duration), time_left(task.time_left), is_stopped(task.is_stopped) {  }
    UserTask& operator = (UserTask const& task) {
        task_id = task.task_id;
        duration = task.duration;
        time_left = task.time_left;
        is_stopped = task.is_stopped;
        return *this;
    }

    void task_step(int time) {
        sleep(1);
        //boost::this_thread::yield();
    }

    // method actually performs user tasks
    void run() {
        is_stopped = false;
        std::cout << "Start user task "<< task_id <<"\n";
        for (time_left = duration; time_left > 0; time_left --) {
            task_step(time_left);
            if (is_stopped) break;
        }
        std::cout << "Finish user task "<< task_id <<"\n";

    }

    void stop() {
        if (!is_stopped)
            std::cout << "stop task " << task_id << "\n";
        is_stopped = true;
    }

  private:
    int task_id;
    int duration;
    int time_left;
    bool is_stopped;
};
