FROM grpc_bench_cpp_base:latest

RUN apt-get update && apt-get install -y \
    libboost-dev

WORKDIR /app

ARG ASIO_GRPC_VERSION=1.2.0
RUN wget --no-verbose https://github.com/Tradias/asio-grpc/archive/refs/tags/v${ASIO_GRPC_VERSION}.tar.gz \
    && tar zxf v${ASIO_GRPC_VERSION}.tar.gz -C ./ \
    && rm v${ASIO_GRPC_VERSION}.tar.gz \
    && cd asio-grpc-${ASIO_GRPC_VERSION} \
    && cmake -B ./build \
    && cmake --build ./build --target install

COPY cpp_asio_grpc_cpp20_coroutine_bench /app

RUN cmake \
        -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/app/out \
    && cmake --build ./build --config Release --parallel --target install

ENTRYPOINT LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.2 /app/out/bin/cpp_asio_grpc_cpp20_coroutine_bench
