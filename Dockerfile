FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    cmake \
    ninja-build \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libxext-dev \
    libgl-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /app
WORKDIR /app

RUN cmake --preset debug-ninja

RUN cmake --build --preset rem8Cpp-debug-ninja

CMD ["ctest", "--test-dir", "build/debug/tests"]
