#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

class ThreadPoolManager;

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
      void set_manager(ThreadPoolManager * manager) {

      }

      void addTask(int task_id) {
        //io_service.post(boost::bind(&UserTask::run, fr));
      }

  private:
    boost::asio::io_service io_service;
    boost::thread_group thread_group;
    std::unique_ptr<boost::asio::io_service::work> work;
    size_t hot_threads;
    ThreadPoolManager * manager_;
};

class UserTask {
  public:
    UserTask(int duration): _duration(duration) {
    }

    // method performs user task
    void run() {
      //thread_pool.add_task(this, root_folder_path);
      //thread_pool.wait_all();
    }

  private:
    int _duration;
};

class ThreadPoolManager {
  public:
    ThreadPoolManager(ThreadPool & pool ): thread_pool(pool) {

    }

    int add_task(int duration) {
      UserTask task(duration);
      task_list.push_back(task);
      thread_pool.addTask(duration);
      //thread_pool.wait_all();
      return 1;
    }
    void remove_task(int task_id) {

    }
  public:
    ThreadPool & thread_pool;
    std::vector<UserTask> task_list;
};
