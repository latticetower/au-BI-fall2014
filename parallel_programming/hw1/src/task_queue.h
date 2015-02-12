#pragma once
#include "headers.h"

template <class UserTask>
class TaskQueue {
public:
  TaskQueue() {

  }

  void add_task(UserTask * task, int key) {
    boost::unique_lock<boost::mutex> lock(mutex_);
    io_mutex.lock();
    std::cout << "add task " << key << " to queue\n";
    io_mutex.unlock();
    task_list.push(task);
    task_condition.notify_one();
  }

  /*method returns operation result and new task*/
  std::pair<bool, UserTask* > get_task(int timeout = -1) {
    boost::unique_lock<boost::mutex> lock(mutex_);

    while (task_list.size() == 0) {
      if (timeout > 0) {
        bool wait_result = task_condition.timed_wait(lock, boost::posix_time::seconds(timeout));
        if (!wait_result) return std::make_pair(false, nullptr);
      } else {
        task_condition.wait(lock);
      }
    }

    auto result = task_list.front();
    task_list.pop();
    return std::make_pair(true, result);
  }
public:
  std::queue<UserTask*> task_list;
  boost::mutex mutex_;
  boost::condition_variable task_condition;
};
