#pragma once
#include "headers.h"
#include "task_performer.h"
#include "user_task.h"
#include "task_queue.h"

class ThreadPool{
  public:
    ThreadPool(int hot_threads_count, int timeout_) {
        hot_threads = hot_threads_count;
        timeout = timeout_;
        free_threads_count.store(0);
        //num_of_threads = boost::thread::hardware_concurrency();
        for (size_t i = 0; i < hot_threads_count; i++) {
          TaskPerformer<ThreadPool> performer(this, -1);
          thread_group.create_thread(boost::bind(&TaskPerformer<ThreadPool>::run_task, &performer));
        }
    }
    void stop() {
        //io_service.stop();
    }

      void wait_all() {
        //work.reset();
        thread_group.join_all();
      }

      ~ThreadPool() {
        stop();
        wait_all();
      }

      int add_task(int task_duration) {
        //io_service.post(boost::bind(&UserTask::run, fr));
        // add new task to all_tasks and get this task_id

        int new_key = get_max_key() + 1;
        all_tasks.insert(std::make_pair(new_key, UserTask(task_duration)));
        task_queue.add_task(&all_tasks[new_key]);
        if (free_threads_count.load() == 0) {
          TaskPerformer<ThreadPool> performer(this, timeout);
          thread_group.create_thread(boost::bind(&TaskPerformer<ThreadPool>::run_task, &performer));
        }
        return new_key;
      }

      //method returns greatest task_id key in all_tasks map
      int get_max_key() {
        int key = 0;
        for (std::map<int, UserTask>::iterator it = all_tasks.begin(); it != all_tasks.end(); ++it) {
            if (it->first >key) key = it->first;
        }
        return key;
      }

      void remove_task(int task_id) {
        if (all_tasks.find(task_id)!=all_tasks.end()) {
          all_tasks[task_id].stop();
        }
        //io_service.post(boost::bind(&UserTask::run, fr));
        //task_queue.add_task(task_id);
      }
      TaskQueue<UserTask> & get_queue() {
        return task_queue;
      }
      void inc_free_threads() {
        ++free_threads_count;
      }

      void dec_free_threads() {
        --free_threads_count;
      }

  private:
    boost::asio::io_service io_service;
    boost::thread_group thread_group;
    std::unique_ptr<boost::asio::io_service::work> work;
    size_t hot_threads;
    int timeout;
    TaskQueue<UserTask> task_queue;
    std::map<int, UserTask> all_tasks;
    boost::atomic<int> free_threads_count;

};
