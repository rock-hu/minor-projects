FROM ubuntu:20.04
WORKDIR /home/openharmony
ARG DEBIAN_FRONTEND=noninteractive
RUN sed -i "s@http://.*archive.ubuntu.com@http://repo.huaweicloud.com@g" /etc/apt/sources.list \
	&& sed -i "s@http://.*security.ubuntu.com@http://repo.huaweicloud.com@g" /etc/apt/sources.list \
	&& mkdir -p /home/tools \
	&& apt-get update -y \
	&& apt-get install -y binutils mtd-utils openssl libssl-dev kmod x11proto-core-dev libc6-dev-i386 bison flex ruby perl cpio gcc git-lfs libtinfo5 libncurses5 g++ libxml2-dev libx11-dev libgl1-mesa-dev lib32z1-dev xsltproc make unzip build-essential u-boot-tools m4 zlib1g-dev gnupg apt-utils genext2fs python3 scons python3-pip python2.7 gcc-arm-none-eabi ccache vim ssh curl wget dosfstools mtools rsync bc libelf-dev default-jre locales doxygen gperf git zip liblz4-tool libpixman-1-dev libglib2.0-dev libxinerama-dev libxcursor-dev libxrandr-dev libxi-dev \
	&& pip3 install --trusted-host https://repo.huaweicloud.com -i https://repo.huaweicloud.com/repository/pypi/simple redis ecdsa pycryptodome \
	&& wget -P /home/tools https://repo.huaweicloud.com/harmonyos/compiler/clang/12.0.1-36cd05/linux/clang_linux-x86_64-36cd05-20221030.tar.bz2 \
	&& wget -P /home/tools https://repo.huaweicloud.com/harmonyos/compiler/ninja/1.10.1/linux/ninja-linux-x86-1.10.1.tar.gz \
	&& wget -P /home/tools https://repo.huaweicloud.com/harmonyos/compiler/gn/2021/linux/gn-linux-x86-2021.tar.gz \
	&& wget -P /home/tools https://mirrors.huaweicloud.com/nodejs/v14.19.1/node-v14.19.1-linux-x64.tar.xz \
	&& locale-gen "en_US.UTF-8" \
	&& rm -rf /bin/sh /usr/bin/python /usr/bin/python3 \
    && mkdir -p /home/tools/gn \
    && mkdir -p /home/tools/ninja \
	&& ln -s /bin/bash /bin/sh \
	&& ln -s /usr/bin/python3.8 /usr/bin/python3 \
	&& ln -s /usr/bin/python3.8 /usr/bin/python \
	&& tar -xvf /home/tools/clang_linux-x86_64-36cd05-20221030.tar.bz2 -C /home/tools \
	&& tar -xvf /home/tools/ninja-linux-x86-1.10.1.tar.gz -C /home/tools/ninja \
	&& tar -xvf /home/tools/gn-linux-x86-2021.tar.gz -C /home/tools/gn \
	&& tar -xJf /home/tools/node-v14.19.1-linux-x64.tar.xz -C /home/tools \
	&& cp /home/tools/node-v14.19.1-linux-x64/bin/node /usr/local/bin \
	&& ln -s /home/tools/node-v14.19.1-linux-x64/lib/node_modules/npm/bin/npm-cli.js /usr/local/bin/npm \
	&& sed -i '$aexport PATH=/home/tools/clang_linux-x86_64-36cd05-20221030/bin:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/ninja:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/gn:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/node-v14.19.1-linux-x64/bin:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/root/.local/bin:$PATH' /root/.bashrc \
	&& export PATH=/home/tools/clang_linux-x86_64-36cd05-20221030/bin:$PATH \
	&& export PATH=/home/tools/ninja:$PATH \
	&& export PATH=/home/tools/node-v14.19.1-linux-x64/bin:$PATH \
	&& export PATH=/home/tools/gn:$PATH \
	&& export PATH=/root/.local/bin:$PATH \
	&& cd /home/openharmony \
	&& rm -rf /home/tools/*.xz \
	&& rm -rf /home/tools/*.gz \
	&& rm -rf /home/tools/*.bz2 \
	&& npm install -g @ohos/hpm-cli --registry https://mirrors.huaweicloud.com/repository/npm/

ENV LANG=en_US.UTF-8 LANGUAGE=en_US.UTF-8 LC_ALL=en_US.UTF-8 TZ=Europe/Moscow
