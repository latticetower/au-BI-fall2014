#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include "thread_pool.h"


bool parse_cmd_options(int argc, char*argv[], int & hot_threads_count, int & timeout) {
  po::options_description desc("Allowed options");
  desc.add_options()
  ("help", "produce help message")
  ("hot_threads_count", po::value <int > (), "number of hot threads in thread pool")
  ("timeout", po::value <int>(), "max lifetime for idle cold thread")
  ;
  po::positional_options_description p;
  p.add("hot_threads_count", 1);
  p.add("timeout", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  if (vm.count("hot_threads_count")) {
    hot_threads_count = vm["hot_threads_count"].as<int>();
  }
  else {
    std::cout  << "hot threads count was not provided\n" << desc << "\n";
    return false;
  }

  if (vm.count("timeout")) {
    timeout = vm["timeout"].as<int>();
  }
  else {
    std::cout  << "timeout was not provided\n" << desc << "\n";
    return false;
  }
  return true;
}


int main(int argc, char* argv[]) {
    //try {
    int hot_threads_count,  timeout;
    if (!parse_cmd_options(argc, argv, hot_threads_count, timeout)) return 1;
    ThreadPool pool(hot_threads_count, timeout);
    ThreadPoolManager pool_manager(pool);
    pool.set_manager(&pool_manager);
    int task_id = pool_manager.add_task(2);
    pool_manager.remove_task(task_id);

      /*}
      catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
      }
      catch(...) {
        std::cerr << "Exception of unknown type!\n";
      }*/

      return 0;
}
