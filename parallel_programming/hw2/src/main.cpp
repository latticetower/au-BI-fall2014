#include <boost/program_options.hpp>
#include <boost/thread.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include <memory>

#include "synchronized_lists.h"


bool parse_cmd_options(int argc, char*argv[], int & readers_count, int & writers_count, int &operations_count, int & list_type) {
  po::options_description desc("Allowed options");
  desc.add_options()
  ("help", "produce help message")
  ("readers_count", po::value <int> (), "number of reader threads")
  ("writers_count", po::value <int>(), "number of writer threads")
  ("operations_count", po::value <int>(), "number of operations")
  ("list_type", po::value <int>(), "list type (0 -locking, otherwise - lock-free)")
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
    operations_count = vm["operations_count"].as<int>();
    list_type = vm["list_type"].as<int>();
  }
  else {
    std::cout  << "not all args were provided\n" << desc << "\n";
    return false;
  }

  return true;
}

template <class ElementType>
void write(SyncList<ElementType>* list) {
}

template <class ElementType>
void read(SyncList<ElementType>* list) {

}


int main(int argc, char* argv[]) {
  //try {
  int rcount, wcount, opcount, list_type;
  if (!parse_cmd_options(argc, argv, rcount, wcount, opcount, list_type)) return 1;
  //std::cout << rcount <<" "<< wcount <<" "<< opcount << " "<< list_type;
  boost::thread_group list_operators;
  SyncList<int>  * list;
  if (list_type) {
    list = new LockFreeList<int>();
  } else {
    list = new LockableList<int>();
  }
  for (int i = 0; i < rcount; i++) {
    list_operators.add_thread(new boost::thread(read<int>, list));
  }

  for (int i = 0; i < wcount; i++) {
    list_operators.add_thread(new boost::thread(write<int>, list));
  }
  list_operators.join_all();
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
