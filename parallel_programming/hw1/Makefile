#BOOST_DIR=/usr/local/include/boost
SRC_DIR=src/
LIBRARIES=-lboost_program_options -lboost_system -lboost_filesystem -lboost_serialization -lboost_thread -lboost_atomic
BUILD_DIR=obj
EXECUTABLES=thread_pool_app
FLAGS=-std=c++0x

all: clean $(BUILD_DIR) $(EXECUTABLES)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/main.o: $(SRC_DIR)main.cpp
	g++ $(FLAGS) -c $< -o $@

thread_pool_app: $(BUILD_DIR)/main.o
	g++ $(FLAGS)  $(BUILD_DIR)/main.o -o $@ $(LIBRARIES)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(EXECUTABLES)
