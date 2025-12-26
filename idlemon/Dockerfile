# docker build -t idlemon_arm7 .
# Linux:   docker run --rm -v "$PWD/output":/output idlemon_arm7 cp idlemon /output/
# Windows: docker run --rm -v ${PWD}/output:/output idlemon_arm7 cp idlemon /output/

FROM arm32v7/debian:buster

# Use archived Debian repos (buster is EOL)
RUN sed -i 's|deb.debian.org|archive.debian.org|g' /etc/apt/sources.list && \
    apt-get -o Acquire::Check-Valid-Until=false update

# Install build tools
RUN apt-get install -y --no-install-recommends \
        gcc \
        libc6-dev \
        make \
        file \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /src

# Copy source
COPY idlemon.c .

# Build binary
RUN gcc -Os -s -o idlemon idlemon.c -pthread

# Default command
CMD ["file", "./idlemon"]
