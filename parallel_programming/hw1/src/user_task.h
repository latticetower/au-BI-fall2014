#pragma once
#include "headers.h"

/* this class encapsulates task functionality*/
class UserTask {
  public:
    UserTask(int task_id_ = 0, int duration_ = 0): task_id(task_id_), duration(duration_), time_left(duration_) {
        is_stopped = false;
    }
    UserTask(UserTask const & task): task_id(task.task_id), duration(task.duration), time_left(task.time_left) {
        is_stopped = task.is_stopped.load();
    }
    UserTask& operator = (UserTask const& task) {
        task_id = task.task_id;
        duration = task.duration;
        time_left = task.time_left;
        is_stopped = task.is_stopped.load();
        return *this;
    }

    void task_step(int time) {
        sleep(1);
        //boost::this_thread::yield();
    }

    // method actually performs user tasks
    void run() {
        if (stopped())
            return; // return immediately if task is stopped
        io_mutex.lock();
        std::cout << "Start user task " << task_id << "\n";
        io_mutex.unlock();
        for (time_left = duration; time_left > 0; time_left --) {
            task_step(time_left);
            if (stopped()) break;
        }
        io_mutex.lock();
        std::cout << "Finish user task " << task_id << "\n";
        io_mutex.unlock();
    }

    void stop() {
        if (!stopped()) {
            io_mutex.lock();
            std::cout << "stop task " << task_id << "\n";
            io_mutex.unlock();
        }
        is_stopped.store(true);
    }

    bool stopped() {
        return is_stopped.load();
    }

  private:
    int task_id;
    int duration;
    int time_left;
    boost::atomic<bool> is_stopped;
};
