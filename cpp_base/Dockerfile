FROM gcc:12

RUN apt-get update && apt-get install -y \
    wget \
    make \
    git \
    libjemalloc-dev

WORKDIR /app

ARG CMAKE_VERSION=3.24.2
RUN wget --no-verbose https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh \
    && chmod +x ./cmake-${CMAKE_VERSION}-linux-x86_64.sh \
    && ./cmake-${CMAKE_VERSION}-linux-x86_64.sh --skip-license --prefix=/usr \
    && rm ./cmake-${CMAKE_VERSION}-linux-x86_64.sh

ARG GRPC_VERSION=1.50.0
RUN git clone --depth 1 --recurse-submodules --branch v${GRPC_VERSION} https://github.com/grpc/grpc.git \
    && cd /app/grpc \
    && cmake -B ./build \
        -DCMAKE_BUILD_TYPE=Release \
        -DgRPC_INSTALL=on \
        -DCARES_BUILD_TOOLS=off \
        -DRE2_BUILD_TESTING=off \
        -DgRPC_BUILD_CSHARP_EXT=off \
        -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=off \
        -DgRPC_BUILD_GRPC_NODE_PLUGIN=off \
        -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=off \
        -DgRPC_BUILD_GRPC_PHP_PLUGIN=off \
        -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=off \
        -DgRPC_BUILD_GRPC_RUBY_PLUGIN=off \
        -DCMAKE_CXX_STANDARD=17 \
    && cmake --build ./build --target install --config Release --parallel=4 \
    && rm -rf /app/grpc

EXPOSE 50051

