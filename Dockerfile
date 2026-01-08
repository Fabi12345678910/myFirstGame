# ---------- Stage 1: Build ----------
FROM gcc:15 AS builder

WORKDIR /build

# Install build tools if needed
RUN apt update
RUN apt install -y cmake make gpp
RUN apt install -y libx11-dev libxrandr-dev libxcursor-dev libxinerama-dev libxi-dev
RUN apt install -y libglvnd-dev libglu1-mesa-dev libglx-dev
RUN apt install -y libudev-dev

# Copy source
COPY include include
COPY src src
COPY CMakeLists.txt CMakeLists.txt

# Build static binary
RUN cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-DENABLE_SERVER_RENDERING=false -DLOG_LEVEL=plog::info"
RUN cmake --build build --target Server --config Release

ENTRYPOINT ["/build/build/Server"]
CMD ["4567"]