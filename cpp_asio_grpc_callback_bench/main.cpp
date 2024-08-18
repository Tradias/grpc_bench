// Copyright 2021 Dennis Hezel
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "helloworld.grpc.pb.h"
#include "streaming/stream.grpc.pb.h"

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/bind_allocator.hpp>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

#include <forward_list>
#include <iostream>
#include <thread>
#include <vector>

void spawn_accept_loop(agrpc::GrpcContext &grpc_context,
                       helloworld::Greeter::AsyncService &service) {
  using RPC =
      agrpc::ServerRPC<&helloworld::Greeter::AsyncService::RequestSayHello>;
  agrpc::register_callback_rpc_handler<RPC>(
      grpc_context, service,
      [](RPC::Ptr ptr, RPC::Request &request) mutable {
        helloworld::HelloReply response;
        *response.mutable_response() = std::move(*request.mutable_request());
        auto &rpc = *ptr;
        rpc.finish(response, grpc::Status::OK, [c = std::move(ptr)](bool) {});
      },
      boost::asio::bind_allocator(grpc_context.get_allocator(),
                            boost::asio::detached));
}

void spawn_accept_loop(agrpc::GrpcContext &grpc_context,
                       streaming::Stream::AsyncService &service) {
  using RPC = agrpc::ServerRPC<
      &streaming::Stream::AsyncService::RequestClientStreaming>;
  agrpc::register_callback_rpc_handler<RPC>(
      grpc_context, service,
      [](RPC::Ptr ptr) mutable {
        auto request_ptr = std::make_unique<streaming::Request>();
        auto &request = *request_ptr;
        auto &rpc = *ptr;
        rpc.read(request, [c = std::move(ptr),
                           p = std::move(request_ptr)](bool) mutable {
          streaming::Response response;
          *response.mutable_response() = std::move(*p->mutable_request());
          auto &rpc = *c;
          rpc.finish(response, grpc::Status::OK, [c = std::move(c)](bool) {});
        });
      },
      boost::asio::bind_allocator(grpc_context.get_allocator(),
                            boost::asio::detached));
}

int main() {
  std::string server_address("0.0.0.0:50051");

  grpc::ServerBuilder builder;
  std::unique_ptr<grpc::Server> server;
  helloworld::Greeter::AsyncService service;
  streaming::Stream::AsyncService streaming_service;

  const auto env = std::getenv("GRPC_SERVER_CPUS");
  const auto parallelism =
      env ? std::atoi(env) : std::thread::hardware_concurrency();
  std::forward_list<agrpc::GrpcContext> grpc_contexts;
  for (size_t i = 0; i < parallelism; ++i) {
    grpc_contexts.emplace_front(builder.AddCompletionQueue());
  }

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  builder.RegisterService(&streaming_service);
  server = builder.BuildAndStart();
  std::cout << "Server listening on " << server_address << std::endl;

  std::vector<std::thread> threads;
  threads.reserve(parallelism);
  for (size_t i = 0; i < parallelism; ++i) {
    threads.emplace_back([&, i] {
      auto &grpc_context = *std::next(grpc_contexts.begin(), i);
      spawn_accept_loop(grpc_context, service);
      spawn_accept_loop(grpc_context, streaming_service);
      grpc_context.run();
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  server->Shutdown();
}
