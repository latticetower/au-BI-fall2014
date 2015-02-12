#pragma once
#include "headers.h"
#include "task_performer.h"
#include "user_task.h"
#include "task_queue.h"

class ThreadPool{
  public:
    ThreadPool(int hot_threads_count, int timeout_) {
        hot_threads = hot_threads_count;
        thread_count = 0;
        timeout = timeout_;
        free_threads_count.store(0);
        kill_signal.store(false);
        for (size_t i = 0; i < hot_threads_count; i++) {
            thread_count ++;
            TaskPerformer<ThreadPool> performer(this, -1, thread_count);
            thread_group.create_thread(performer);
        }
    }

    void stop() {
        remove_all_tasks();
        kill_signal.store(true);

    }

    void wait_all() {
        thread_group.join_all();
    }

    ~ThreadPool() {
        stop();
        thread_group.interrupt_all();
        wait_all();
    }

    int add_task(int task_duration) {
        // add new task to all_tasks and get this task_id

        int new_key = get_max_key() + 1;
        all_tasks.insert(std::make_pair(new_key, UserTask(new_key, task_duration)));
        task_queue.add_task(&all_tasks[new_key], new_key);
        if (free_threads_count.load() == 0) {
            io_mutex.lock();
            std::cout << "Adding temporary cold thread with timeout " << " " << timeout << "\n";
            io_mutex.unlock();
            thread_count ++;
            TaskPerformer<ThreadPool> performer(this, timeout, thread_count);
            thread_group.create_thread(performer);
        }
        return new_key;
    }

    //method returns greatest task_id key in all_tasks map
    int get_max_key() {
        int key = 0;
        for (std::map<int, UserTask>::iterator it = all_tasks.begin(); it != all_tasks.end(); ++it) {
            if (it->first > key) key = it->first;
        }
        return key;
    }

    void remove_task(int task_id) {
        if (all_tasks.find(task_id)!=all_tasks.end()) {
            all_tasks[task_id].stop();
        }
    }
    void remove_all_tasks() {
      for (auto iter = all_tasks.begin();iter!= all_tasks.end(); ++iter) {
        iter->second.stop();
      }
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
    bool received_kill_signal() {
        return kill_signal.load();
    }

  private:
    boost::thread_group thread_group;
    int hot_threads;
    int timeout;
    std::map<int, UserTask> all_tasks;//contains all task with corresponding ids
    TaskQueue<UserTask> task_queue;
    boost::atomic<int> free_threads_count;
    boost::atomic<bool> kill_signal;
    int thread_count;
};
