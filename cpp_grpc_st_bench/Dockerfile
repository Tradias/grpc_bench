FROM grpc-bench-cpp-base:latest

WORKDIR /app

COPY proto /app/proto
RUN mkdir gen \
    && protoc --proto_path=/app/proto/helloworld --cpp_out=gen helloworld.proto \
    && protoc --proto_path=/app/proto/helloworld --grpc_out=gen --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` helloworld.proto

COPY cpp_grpc_st_bench /app

RUN cmake \
        -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/app/out \
    && cmake --build ./build --config Release --target install

ENTRYPOINT LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.2 /app/out/bin/cpp_grpc_st_bench
