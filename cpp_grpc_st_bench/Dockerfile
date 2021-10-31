FROM grpc_bench_cpp_base:latest

WORKDIR /app

COPY cpp_grpc_st_bench /app

RUN cmake \
        -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/app/out \
    && cmake --build ./build --config Release --parallel --target install

ENTRYPOINT LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.2 /app/out/bin/cpp_grpc_st_bench
