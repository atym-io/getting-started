# [Parameters]

# 'VERSION' is passed through the `devcontainer.json` file. Change version there
ARG VERSION
FROM ghcr.io/atym-io/atym-c-toolchain:${VERSION}

ARG HOSTTYPE
ENV HOSTTYPE=$HOSTTYPE
ARG ZSDK_VERSION=0.16.8
ENV ZSDK_VERSION=$ZSDK_VERSION
ARG WGET_ARGS="-q --show-progress --progress=bar:force:noscroll"

# Avoid interactive prompts during package install
ENV DEBIAN_FRONTEND=noninteractive

# Install apt packages
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    git cmake ninja-build gperf \
    ccache dfu-util device-tree-compiler wget \
    python3-dev python3-pip python3-setuptools python3-tk python3-wheel \
    xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1 \
	&& apt-get clean && rm -rf /var/lib/apt/lists/*

	# Install Zephyr SDK
# If aarch64 install, remove '-h' flag from install script as host tools do not have a candidate
RUN mkdir -p /opt/toolchains && \
	cd /opt/toolchains && \
	wget ${WGET_ARGS} https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZSDK_VERSION}/zephyr-sdk-${ZSDK_VERSION}_linux-${HOSTTYPE}.tar.xz && \
	tar xf zephyr-sdk-${ZSDK_VERSION}_linux-${HOSTTYPE}.tar.xz && \
	zephyr-sdk-${ZSDK_VERSION}/setup.sh -t all -h -c && \
	rm zephyr-sdk-${ZSDK_VERSION}_linux-${HOSTTYPE}.tar.xz

# Run the Zephyr SDK setup script as 'vscode' in order to ensure that the
# `Zephyr-sdk` CMake package is located in the package registry under the
# user's home directory.
USER vscode

ARG USERNAME
ENV USERNAME=$USERNAME
RUN sudo -E -- bash -c ' \
	/opt/toolchains/zephyr-sdk-${ZSDK_VERSION}/setup.sh -c && \
	chown -R ${USERNAME}:${USERNAME} /home/${USERNAME}/.cmake \
	'

# Install west via pip
RUN pip3 install --no-cache-dir west pyelftools