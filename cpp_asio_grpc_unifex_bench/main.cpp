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

#include <agrpc/asio_grpc.hpp>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <unifex/get_allocator.hpp>
#include <unifex/just.hpp>
#include <unifex/let_value_with.hpp>
#include <unifex/sync_wait.hpp>
#include <unifex/then.hpp>
#include <unifex/when_all.hpp>
#include <unifex/with_query_value.hpp>

#include <forward_list>
#include <iostream>
#include <thread>
#include <vector>

auto spawn_accept_loop(agrpc::GrpcContext &grpc_context,
                       helloworld::Greeter::AsyncService &service) {
  using RPC =
      agrpc::ServerRPC<&helloworld::Greeter::AsyncService::RequestSayHello>;
  return unifex::with_query_value(
      agrpc::register_sender_rpc_handler<RPC>(
          grpc_context, service,
          [&](RPC &rpc, helloworld::HelloRequest &request) {
            return unifex::let_value_with(
                [] { return helloworld::HelloReply{}; },
                [&](auto &response) {
                  *response.mutable_response() =
                      std::move(*request.mutable_request());
                  return rpc.finish(response, grpc::Status::OK);
                });
          }),
      unifex::get_allocator, grpc_context.get_allocator());
}

#if 0
auto spawn_accept_loop(agrpc::GrpcContext &grpc_context,
                       helloworld::Greeter::AsyncService &service) {
  return unifex::with_query_value(
      agrpc::repeatedly_request(
          &helloworld::Greeter::AsyncService::RequestSayHello, service,
          [&](grpc::ServerContext &, helloworld::HelloRequest &request,
              grpc::ServerAsyncResponseWriter<helloworld::HelloReply> &writer) {
            return unifex::let_value(
                unifex::just(helloworld::HelloReply{}), [&](auto &response) {
                  *response.mutable_response() =
                      std::move(*request.mutable_request());
                  return agrpc::finish(writer, response, grpc::Status::OK,
                                       agrpc::use_sender(grpc_context));
                });
          },
          agrpc::use_sender(grpc_context)),
      unifex::get_allocator, grpc_context.get_allocator());
}
#endif

auto run_thread(agrpc::GrpcContext &grpc_context,
                helloworld::Greeter::AsyncService &service) {
  return unifex::when_all(
      spawn_accept_loop(grpc_context, service),
      unifex::then(unifex::just(), [&]() { grpc_context.run(); }));
}

int main() {
  std::string server_address("0.0.0.0:50051");

  grpc::ServerBuilder builder;
  std::unique_ptr<grpc::Server> server;
  helloworld::Greeter::AsyncService service;

  const auto env = std::getenv("GRPC_SERVER_CPUS");
  const auto parallelism =
      env ? std::atoi(env) : std::thread::hardware_concurrency();
  std::forward_list<agrpc::GrpcContext> grpc_contexts;
  for (size_t i = 0; i < parallelism; ++i) {
    grpc_contexts.emplace_front(builder.AddCompletionQueue());
  }

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  server = builder.BuildAndStart();
  std::cout << "Server listening on " << server_address << std::endl;

  std::vector<std::thread> threads;
  threads.reserve(parallelism);

  for (size_t i = 0; i < parallelism; ++i) {
    threads.emplace_back([&, i] {
      auto &grpc_context = *std::next(grpc_contexts.begin(), i);
      unifex::sync_wait(run_thread(grpc_context, service));
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  server->Shutdown();
}
