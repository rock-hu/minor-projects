FROM ubuntu:20.04
WORKDIR /home/openharmony
ARG DEBIAN_FRONTEND=noninteractive
RUN sed -i "s@http://.*archive.ubuntu.com@http://repo.huaweicloud.com@g" /etc/apt/sources.list \
	&& sed -i "s@http://.*security.ubuntu.com@http://repo.huaweicloud.com@g" /etc/apt/sources.list \
	&& mkdir -p /home/tools \
	&& apt-get update -y \
	&& apt-get install -y binutils bison flex mtd-utils python3 ruby scons perl openssl libssl-dev cpio git-lfs ccache vim ssh doxygen gperf libx11-dev libgl1-mesa-dev lib32z1-dev xsltproc x11proto-core-dev libc6-dev-i386 curl make git zip wget dosfstools mtools unzip bc build-essential u-boot-tools python3-pip m4 zlib1g-dev libelf-dev gnupg default-jre locales default-jdk python3.8-distutils libffi-dev zlib* e2fsprogs device-tree-compiler git-core libdwarf-dev gcc g++ lib32ncurses5-dev gnutls-bin gcc-arm-linux-gnueabi \
	&& pip3 install --trusted-host https://repo.huaweicloud.com -i https://repo.huaweicloud.com/repository/pypi/simple ecdsa kconfiglib pycryptodome setuptools ohos-build requests \
	&& wget -P /home/tools https://repo.huaweicloud.com/harmonyos/compiler/hc-gen/0.65/linux/hc-gen-0.65-linux.tar \
	&& wget -P /home/tools https://repo.huaweicloud.com/harmonyos/compiler/gcc_riscv32/7.3.0/linux/gcc_riscv32-linux-7.3.0.tar.gz \
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
	&& tar -xvf /home/tools/hc-gen-0.65-linux.tar -C /home/tools \
	&& tar -xvf /home/tools/gcc_riscv32-linux-7.3.0.tar.gz -C /home/tools \
	&& tar -xvf /home/tools/ninja-linux-x86-1.10.1.tar.gz -C /home/tools/ninja \
	&& tar -xvf /home/tools/gn-linux-x86-2021.tar.gz -C /home/tools/gn \
	&& tar -xJf /home/tools/node-v14.19.1-linux-x64.tar.xz -C /home/tools \
	&& cp /home/tools/node-v14.19.1-linux-x64/bin/node /usr/local/bin \
	&& ln -s /home/tools/node-v14.19.1-linux-x64/lib/node_modules/npm/bin/npm-cli.js /usr/local/bin/npm \
	&& ln -s /home/tools/node-v14.19.1-linux-x64/lib/node_modules/npm/bin/npx-cli.js /usr/local/bin/npx \
	&& sed -i '$aexport PATH=/home/tools/hc-gen:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/gcc_riscv32/bin:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/ninja:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/gn:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/node-v14.19.1-linux-x64/bin:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/root/.local/bin:$PATH' /root/.bashrc \
	&& export PATH=/home/tools/hc-gen:$PATH \
	&& export PATH=/home/tools/gcc_riscv32/bin:$PATH \
	&& export PATH=/home/tools/ninja:$PATH \
	&& export PATH=/home/tools/node-v14.19.1-linux-x64/bin:$PATH \
	&& export PATH=/home/tools/gn:$PATH \
	&& export PATH=/root/.local/bin:$PATH \
	&& rm -rf /home/tools/*.xz \
	&& rm -rf /home/tools/*.gz \
	&& rm -rf /home/tools/*.tar \
	&& npm install -g @ohos/hpm-cli --registry https://mirrors.huaweicloud.com/repository/npm/

ENV LANG=en_US.UTF-8 LANGUAGE=en_US.UTF-8 LC_ALL=en_US.UTF-8 TZ=Europe/Moscow
