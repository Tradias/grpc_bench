FROM grpc_bench_cpp_base:latest

RUN apt-get update && apt-get install -y \
    git curl zip unzip tar

WORKDIR /app

RUN git clone https://github.com/Tradias/asio-grpc.git \
    && cd asio-grpc \
    && git checkout 0920281f4cb992ceb1bbfdd0298c1886de71f103 \
    && cmake -B ./build \
    && cmake --build ./build --target install
    
RUN git clone https://github.com/microsoft/vcpkg.git \
    && ./vcpkg/bootstrap-vcpkg.sh \
    && ./vcpkg/vcpkg install libunifex \
    && ./vcpkg/vcpkg integrate install

COPY cpp_asio_grpc_unifex_bench /app
RUN wget https://raw.githubusercontent.com/facebookexperimental/libunifex/main/cmake/FindCoroutines.cmake

RUN cmake \
        -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/app/out \
        -DCMAKE_TOOLCHAIN_FILE=/app/vcpkg/scripts/buildsystems/vcpkg.cmake \
        -DCMAKE_MODULE_PATH=/app \
        -DCMAKE_CXX_STANDARD=20 \
    && cmake --build ./build --config Release --parallel --target install

ENTRYPOINT LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.2 /app/out/bin/cpp_asio_grpc_unifex_bench
