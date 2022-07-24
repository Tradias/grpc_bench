FROM grpc-bench-cpp-base:latest

RUN apt-get update && apt-get install -y \
    git curl zip unzip tar

WORKDIR /app

RUN git clone https://github.com/Tradias/asio-grpc.git \
    && cd asio-grpc \
    && git checkout v2.0.0 \
    && cmake -B ./build \
    && cmake --build ./build --target install

RUN git clone https://github.com/microsoft/vcpkg.git \
    && ./vcpkg/bootstrap-vcpkg.sh \
    && ./vcpkg/vcpkg install boost-asio liburing \
    && ./vcpkg/vcpkg integrate install

COPY proto /app/proto
RUN mkdir gen \
    && protoc --proto_path=/app/proto/helloworld --cpp_out=gen helloworld.proto \
    && protoc --proto_path=/app/proto/helloworld --grpc_out=gen --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` helloworld.proto

COPY cpp_asio_grpc_io_context_coro_bench /app

RUN cmake \
        -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/app/out \
        -DCMAKE_TOOLCHAIN_FILE=/app/vcpkg/scripts/buildsystems/vcpkg.cmake \
    && cmake --build ./build --config Release --target install

ENTRYPOINT LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.2 /app/out/bin/cpp_asio_grpc_io_context_coro_bench
