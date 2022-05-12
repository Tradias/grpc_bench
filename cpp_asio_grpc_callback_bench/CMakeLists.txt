cmake_minimum_required(VERSION 3.15)

project(
  cpp_asio_grpc_callback_bench
  DESCRIPTION "Benchmark for asio-grpc with callbacks"
  HOMEPAGE_URL https://github.com/Tradias/asio-grpc
  LANGUAGES CXX)

find_package(protobuf REQUIRED)
find_package(asio-grpc CONFIG REQUIRED)

include(FindPkgConfig)
pkg_check_modules(jemalloc IMPORTED_TARGET GLOBAL jemalloc)
pkg_search_module(jemalloc REQUIRED jemalloc)


add_executable(${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PRIVATE main.cpp "${CMAKE_CURRENT_LIST_DIR}/gen/helloworld.pb.cc"
          "${CMAKE_CURRENT_LIST_DIR}/gen/helloworld.grpc.pb.cc")

target_link_libraries(${PROJECT_NAME} PRIVATE asio-grpc::asio-grpc jemalloc)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME}
                           PRIVATE "${CMAKE_CURRENT_LIST_DIR}/gen")

target_compile_options(${PROJECT_NAME} PRIVATE -flto)

target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_ASIO_NO_DEPRECATED)

install(TARGETS ${PROJECT_NAME})
