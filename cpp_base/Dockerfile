FROM gcc:11

RUN apt-get update && apt-get install -y \
    wget \
    make \
    git \
    libjemalloc2

WORKDIR /app

ARG CMAKE_VERSION=3.21.2
RUN wget --no-verbose https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh \
    && chmod +x ./cmake-${CMAKE_VERSION}-linux-x86_64.sh \
    && ./cmake-${CMAKE_VERSION}-linux-x86_64.sh --skip-license --prefix=/usr \
    && rm ./cmake-${CMAKE_VERSION}-linux-x86_64.sh

ARG GRPC_VERSION=1.41.0
COPY cpp_base/boringssl_numeric_limits.patch /app
RUN git clone --depth 1 --recurse-submodules --branch v${GRPC_VERSION} https://github.com/grpc/grpc.git \
    && cd grpc/third_party/boringssl-with-bazel \
    && git apply /app/boringssl_numeric_limits.patch \
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
    && cmake --build ./build --target install --parallel=4 \
    && rm -rf /app/grpc

COPY proto /app/proto
RUN mkdir gen \
    && protoc --proto_path=/app/proto/helloworld --cpp_out=gen helloworld.proto \
    && protoc --proto_path=/app/proto/helloworld --grpc_out=gen --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` helloworld.proto

EXPOSE 50051

