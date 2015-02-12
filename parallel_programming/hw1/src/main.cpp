
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
    io_mutex.lock();
    std::cout << desc << "\n";
    io_mutex.unlock();
    return 0;
  }

  if (vm.count("hot_threads_count") && vm.count("timeout")) {
    hot_threads_count = vm["hot_threads_count"].as<int>();
    timeout = vm["timeout"].as<int>();
  }
  else {
    io_mutex.lock();
    std::cout  << "some arguments were not provided\n" << desc << "\n";
    io_mutex.unlock();
    return false;
  }

  return true;
}

//helper method - for parsing line with new tasks
void add_all_tasks(std::vector<std::string> const & tokens, ThreadPool & pool) {
    if (tokens[0] == "add") {
        //assume that 2nd and all next tokens are timeouts for new tasks
        for (int i = 1; i < tokens.size(); i++) {
            int timeout = stoi(tokens[i]);
            if (timeout > 0) {
                pool.add_task(timeout);
                //std::cout << "Added new task with id = " << task_id << std::endl;
            }
        }
    }
}

//helper method - for parsing obsolete-and-should-be-removed tasks
void remove_all_tasks(std::vector<std::string> const & tokens, ThreadPool & pool) {
    if (tokens[0] == "remove") {
        //assume that 2nd and all next tokens are task ids
        for (int i = 1; i < tokens.size(); i++) {
            int task_id = stoi(tokens[i]);
            if (task_id > 0) {
                pool.remove_task(task_id);
                io_mutex.lock();
                std::cout << "Trying to remove task with id = " << task_id << std::endl;
                io_mutex.unlock();
            }
        }
    }
}

//helper method to show help message about cmd options
void show_help_in_cmdline(std::vector<std::string> const& tokens) {
    if (tokens[0] == "help") {
      io_mutex.lock();
      std::cout << "commands:\n" <<
          "\"add <time1> <time2> ...\" -  to add set of new tasks with given time intervals\n" <<
          "\"remove <task_id1> <task_id2> ...\" -  to remove set of tasks with given ids\n" <<
          "\"exit\" -  to exit\n";
      io_mutex.unlock();
    }
}
void signal_callback_handler(int signum) {
    io_mutex.lock();
    std::cout << "Interrupted, closing...\n ";
    io_mutex.unlock();
    exit(signum);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signal_callback_handler);
    signal(SIGKILL, signal_callback_handler);

    //try {
    int hot_threads_count,  timeout;
    if (!parse_cmd_options(argc, argv, hot_threads_count, timeout)) return 1;
    ThreadPool pool(hot_threads_count, timeout);
    std::string input_str;
    while (true) {
        io_mutex.lock();
        std::getline(std::cin, input_str);
        io_mutex.unlock();
        std::vector<std::string> tokens;
        boost::split(tokens, input_str, boost::is_any_of("\t "));
        show_help_in_cmdline(tokens);
        if (tokens.size() > 1) {
            add_all_tasks(tokens, pool);
            remove_all_tasks(tokens, pool);
        } //else {/* do nothing */  }
        if (tokens[0] == "exit")
            break;
    }
    pool.stop();
    io_mutex.lock();
    std::cout << "Got exit signal\n";
    io_mutex.unlock();
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
