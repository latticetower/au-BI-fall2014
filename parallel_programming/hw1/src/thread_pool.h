#pragma once
#include "headers.h"
#include "task_performer.h"
#include "user_task.h"
#include "task_queue.h"

class ThreadPool{
  public:
    ThreadPool(int hot_threads_count, int timeout): io_service(), work(new boost::asio::io_service::work(io_service)) {
      //num_of_threads = boost::thread::hardware_concurrency();
      for (size_t i = 0; i < hot_threads_count; i++)
        thread_group.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));
      }

      void stop() {
        io_service.stop();
      }

      void wait_all() {
        work.reset();
        thread_group.join_all();
      }

      ~ThreadPool() {
        stop();
        wait_all();
      }

      void add_task(int task_id) {
        //io_service.post(boost::bind(&UserTask::run, fr));
        task_queue.add_task(task_id);
      }

      void remove_task(int task_id) {
        //io_service.post(boost::bind(&UserTask::run, fr));
        //task_queue.add_task(task_id);
      }

  private:
    boost::asio::io_service io_service;
    boost::thread_group thread_group;
    std::unique_ptr<boost::asio::io_service::work> work;
    size_t hot_threads;
    TaskQueue<UserTask> task_queue;

};
