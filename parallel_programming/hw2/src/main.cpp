#include <boost/program_options.hpp>
#include <boost/thread.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include <memory>
#include <atomic>

#include "base.h"
#include "lockable_list.h"
#include "lock_free_list.h"

boost::atomic<long long> opcount;

bool parse_cmd_options(int argc, char*argv[], int & readers_count, int & writers_count, long long &operations_count, int & list_type) {
  po::options_description desc("Allowed options");
  desc.add_options()
  ("help", "produce help message")
  ("readers_count", po::value <int> (), "number of reader threads")
  ("writers_count", po::value <int>(), "number of writer threads")
  ("operations_count", po::value <long long>(), "number of operations")
  ("list_type", po::value <int>(), "list type (0 -locking, 1 - lock-free)")
  ;
  po::positional_options_description p;
  p.add("readers_count", 1);
  p.add("writers_count", 1);
  p.add("operations_count", 1);
  p.add("list_type", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  if (vm.count("readers_count") &&
      vm.count("writers_count") &&
      vm.count("operations_count") &&
      vm.count("list_type")) {
        readers_count = vm["readers_count"].as<int>();
        writers_count = vm["writers_count"].as<int>();
        operations_count = vm["operations_count"].as<long long>();
        list_type = vm["list_type"].as<int>();
  }
  else {
    std::cout  << "not all args were provided\n" << desc << "\n";
    return false;
  }

  return true;
}

/* operation_type == 1 if writer, otherwise 0 (reader)
*/
template <class ElementType>
void do_smth(boost::shared_ptr<SyncList<ElementType> > list, int operation_type, int i) {
  if (operation_type) {//write mode
    while (opcount.load() > 0) {
      opcount --;
      int value = rand() % 1000;
      KeyType key = (KeyType)value;
      bool optype = (rand() % 100) > 20;

      // let's call add if given random number % 100 is greater than 20, else call erase
      if (optype) {
        list->insert(value, key);
      }
      else {
        list->erase(key);
      }
      std::cout << "thread "<< i << "\n";
    }
  }
  else {//read mode
    while (opcount.load() > 0) {
      opcount --;
      KeyType key_to_find = rand() % 1000;
      std::pair<bool, int> find_result = list->find(key_to_find);
      std::cout << "thread " << i << " \n";
      //if (find_result->first) {
      //  std::cout << "smth was found\n";
      //}
    }
  }
}



int main(int argc, char* argv[]) {
    srand(time(NULL));

    int rcount, wcount, list_type;
    long long operations_count;

    if (!parse_cmd_options(argc, argv, rcount, wcount, operations_count, list_type)) return 1;
    opcount = operations_count;
    boost::thread_group list_operators;
    boost::shared_ptr<SyncList<int> > list;
    if (list_type) {
        list = boost::shared_ptr<LockFreeList<int> >(new LockFreeList<int>());
    } else {
        list = boost::shared_ptr<LockableList<int> >(new LockableList<int>());
    }
    for (int i = 0; i < rcount + wcount; i++) {
        list_operators.add_thread(new boost::thread(do_smth<int>, list, i < wcount, i));
    }
    list_operators.join_all();

    return 0;
}
