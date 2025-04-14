FROM debian:bookworm-slim
ENV DEBIAN_FRONTEND=noninteractive

ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get -y update && apt-get -y install \
	bc \
    build-essential \
    bzip2 \
	bzr \
	cmake \
	cmake-curses-gui \
	cpio \
	device-tree-compiler \
	git \
	golang \
	libncurses5-dev \
	libsdl1.2-dev \
	libsdl-image1.2-dev \
	libsdl-ttf2.0-dev \
	libsdl2-dev \
	libsdl2-image-dev \
	libsdl2-ttf-dev \
	locales \
	make \
	rsync \
	scons \
	squashfs-tools \
	tree \
	unzip \
	wget \
  && rm -rf /var/lib/apt/lists/*

COPY support .
RUN chmod 777 /setup-toolchain.sh
RUN chmod 777 /setup-env.sh
RUN ./setup-toolchain.sh
RUN cat setup-env.sh >> ~/.bashrc

RUN mkdir -p /root/workspace
WORKDIR /root

VOLUME /root/workspace
WORKDIR /root/workspace

CMD ["/bin/bash"]