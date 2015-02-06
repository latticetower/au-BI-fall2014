
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

  if (vm.count("hot_threads_count") && vm.count("timeout")) {
    hot_threads_count = vm["hot_threads_count"].as<int>();
    timeout = vm["timeout"].as<int>();
  }
  else {
    std::cout  << "some arguments were not provided\n" << desc << "\n";
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
        //int task_id =
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
        std::cout << "Trying to remove task with id = " << task_id << std::endl;
      }
    }
  }
}

//helper method to show help message about cmd options
void show_help_in_cmdline(std::vector<std::string> const& tokens) {
  if (tokens[0] == "help") {
    std::cout << "commands:\n" <<
        "\"add <time1> <time2> ...\" -  to add set of new tasks with given time intervals\n" <<
        "\"remove <task_id1> <task_id2> ...\" -  to remove set of tasks with given ids\n" <<
        "\"exit\" -  to exit\n";
  }
}

int main(int argc, char* argv[]) {
    //try {
    int hot_threads_count,  timeout;
    if (!parse_cmd_options(argc, argv, hot_threads_count, timeout)) return 1;
    ThreadPool pool(hot_threads_count, timeout);
    std::string input_str;
    while (true) {
        std::getline(std::cin, input_str);
        std::vector<std::string> tokens;
        boost::split(tokens, input_str, boost::is_any_of("\t "));
        std::cout << tokens.size() << " " << tokens[0];
        show_help_in_cmdline(tokens);
        if (tokens.size() > 1) {
          add_all_tasks(tokens, pool);
          remove_all_tasks(tokens, pool);
        } //else {/* do nothing */  }
        if (tokens[0] == "exit")
          break;
    }
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
