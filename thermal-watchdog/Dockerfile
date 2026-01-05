FROM rust AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates curl xz-utils \
  && rm -rf /var/lib/apt/lists/*

ARG ZIG_VERSION=0.15.2
RUN curl -fsSL "https://ziglang.org/download/${ZIG_VERSION}/zig-x86_64-linux-${ZIG_VERSION}.tar.xz" \
  | tar -xJ -C /opt \
 && ln -s "/opt/zig-x86_64-linux-${ZIG_VERSION}/zig" /usr/local/bin/zig

RUN rustup target add aarch64-unknown-linux-gnu && \
    cargo install --locked cargo-zigbuild

WORKDIR /app

COPY Cargo.toml Cargo.lock ./
# If workspace: copy each member's Cargo.toml too.
COPY src ./src
RUN cargo zigbuild --release --target aarch64-unknown-linux-gnu.2.33

FROM scratch AS artifact
COPY --from=build /app/target/aarch64-unknown-linux-gnu/release/thermal-watchdog /thermal-watchdog
