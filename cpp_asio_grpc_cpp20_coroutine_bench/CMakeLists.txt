cmake_minimum_required(VERSION 3.15)

project(
  cpp_asio_grpc_cpp20_coroutine_bench
  DESCRIPTION "Benchmark for asio-grpc with C++20 coroutines"
  HOMEPAGE_URL https://github.com/Tradias/asio-grpc
  LANGUAGES CXX)

find_package(protobuf REQUIRED)
find_package(asio-grpc REQUIRED)

add_executable(${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PRIVATE main.cpp "${CMAKE_CURRENT_LIST_DIR}/gen/helloworld.pb.cc"
          "${CMAKE_CURRENT_LIST_DIR}/gen/helloworld.grpc.pb.cc")

target_link_libraries(${PROJECT_NAME} PRIVATE asio-grpc::asio-grpc)

target_include_directories(${PROJECT_NAME}
                           PRIVATE "${CMAKE_CURRENT_LIST_DIR}/gen")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_compile_options(${PROJECT_NAME} PRIVATE -flto -fcoroutines)

target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_ASIO_NO_DEPRECATED)

install(TARGETS ${PROJECT_NAME})
