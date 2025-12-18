FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    build-essential \
    git \
    pkg-config \
    libffi-dev \
    libssl-dev \
    python3 \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /build

RUN git clone https://github.com/micropython/micropython.git

WORKDIR /build/micropython

RUN make -C mpy-cross

WORKDIR /build/micropython/ports/unix
RUN make submodules && make
