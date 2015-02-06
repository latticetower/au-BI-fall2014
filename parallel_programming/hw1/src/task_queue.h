#pragma once
#include "headers.h"

template <class UserTask>
class TaskQueue {
public:
  TaskQueue() {

  }

  void add_task(int duration) {
    boost::unique_lock<boost::mutex> lock(mutex_);
    std::cout <<"add task to queue\n";
    task_list.push(std::shared_ptr<UserTask>(new UserTask(duration)));
    task_condition.notify_one();
  }

  /*method returns operation result and new task*/
  std::pair<bool, std::shared_ptr<UserTask> > get_task(int task_id, int timeout = -1) {
    boost::unique_lock<boost::mutex> lock(mutex_);

    while (task_list.size() == 0) {
      if (timeout > 0) {
        bool wait_result = task_condition.timed_wait(lock, timeout);
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
  std::queue<std::shared_ptr<UserTask> > task_list;
  boost::mutex mutex_;
  boost::condition_variable task_condition;
};
