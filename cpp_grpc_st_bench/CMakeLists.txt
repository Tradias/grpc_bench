cmake_minimum_required(VERSION 3.15)

project(
  cpp_grpc_st_bench
  LANGUAGES CXX)

find_package(protobuf REQUIRED)
find_package(gRPC REQUIRED)

add_executable(${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PRIVATE main.cpp "${CMAKE_CURRENT_LIST_DIR}/gen/helloworld.pb.cc"
          "${CMAKE_CURRENT_LIST_DIR}/gen/helloworld.grpc.pb.cc")

target_link_libraries(${PROJECT_NAME} PRIVATE gRPC::grpc++_unsecure)

target_include_directories(${PROJECT_NAME}
                           PRIVATE "${CMAKE_CURRENT_LIST_DIR}/gen")

target_compile_options(${PROJECT_NAME} PRIVATE -flto)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

install(TARGETS ${PROJECT_NAME})
