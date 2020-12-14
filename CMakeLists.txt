cmake_minimum_required(VERSION 3.17)
project(priorityServerCrow)

set(CMAKE_CXX_STANDARD 17)


find_package(Boost 1.65.1 COMPONENTS system filesystem  REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(priorityServerCrow main.cpp)
target_link_libraries(priorityServerCrow ${Boost_LIBRARIES})
target_link_libraries(priorityServerCrow Threads::Threads)
