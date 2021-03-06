#pragma once

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <string>
#include <iterator>
#include <queue>
#include <map>

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/atomic.hpp>

#include <signal.h>

boost::mutex io_mutex;
