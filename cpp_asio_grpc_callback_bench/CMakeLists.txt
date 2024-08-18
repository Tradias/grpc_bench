cmake_minimum_required(VERSION 3.15)

project(
  cpp_asio_grpc_callback_bench
  DESCRIPTION "Benchmark for asio-grpc with callbacks"
  HOMEPAGE_URL https://github.com/Tradias/asio-grpc
  LANGUAGES CXX)

find_package(protobuf REQUIRED)
find_package(asio-grpc CONFIG REQUIRED)
find_package(Boost REQUIRED)

add_executable(${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PRIVATE main.cpp
          "${CMAKE_CURRENT_LIST_DIR}/gen/helloworld.pb.cc"
          "${CMAKE_CURRENT_LIST_DIR}/gen/helloworld.grpc.pb.cc"
          "${CMAKE_CURRENT_LIST_DIR}/gen/grpc/health/v1/health.pb.cc"
          "${CMAKE_CURRENT_LIST_DIR}/gen/grpc/health/v1/health.grpc.pb.cc"
          "${CMAKE_CURRENT_LIST_DIR}/gen/streaming/stream.pb.cc"
          "${CMAKE_CURRENT_LIST_DIR}/gen/streaming/stream.grpc.pb.cc")

target_link_libraries(${PROJECT_NAME} PRIVATE asio-grpc::asio-grpc jemalloc Boost::headers)

target_include_directories(${PROJECT_NAME}
                           PRIVATE "${CMAKE_CURRENT_LIST_DIR}/gen")

target_compile_options(${PROJECT_NAME} PRIVATE -flto)

target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_ASIO_NO_DEPRECATED)

install(TARGETS ${PROJECT_NAME})
