FROM ubuntu:20.04
WORKDIR /home/openharmony
ARG DEBIAN_FRONTEND=noninteractive
RUN sed -i "s@http://.*archive.ubuntu.com@http://repo.huaweicloud.com@g" /etc/apt/sources.list \
	&& sed -i "s@http://.*security.ubuntu.com@http://repo.huaweicloud.com@g" /etc/apt/sources.list \
	&& mkdir -p /home/tools \
	&& mkdir -p /home/tools/gn \
	&& apt-get update -y \
	&& apt-get install -y binutils mtd-utils openssl libssl-dev kmod x11proto-core-dev libc6-dev-i386 bison flex ruby perl cpio gcc git-lfs libtinfo5 libncurses5 g++ libxml2-dev libx11-dev libgl1-mesa-dev lib32z1-dev xsltproc make unzip build-essential u-boot-tools m4 zlib1g-dev gnupg apt-utils genext2fs python3.8 scons python3-pip python2.7 gcc-arm-none-eabi ccache vim ssh curl wget dosfstools mtools rsync bc libelf-dev default-jre locales liblz4-tool libpixman-1-dev libglib2.0-dev doxygen gperf git zip default-jdk python3.8-distutils libffi-dev zlib* e2fsprogs device-tree-compiler lib32ncurses5-dev gnutls-bin git-core libdwarf-dev libxinerama-dev libxcursor-dev libxrandr-dev libxi-dev \ 
	&& pip3 install --trusted-host https://repo.huaweicloud.com -i https://repo.huaweicloud.com/repository/pypi/simple requests redis ecdsa kconfiglib pycryptodome setuptools ohos-build \
	&& wget -P /home/tools https://repo.huaweicloud.com/harmonyos/compiler/clang/10.0.1-62608/linux/llvm.tar.gz \
	&& wget -P /home/tools https://repo.huaweicloud.com/harmonyos/compiler/hc-gen/0.65/linux/hc-gen-0.65-linux.tar \
	&& wget -P /home/tools https://repo.huaweicloud.com/harmonyos/compiler/gcc_riscv32/7.3.0/linux/gcc_riscv32-linux-7.3.0.tar.gz \
	&& wget -P /home/tools https://repo.huaweicloud.com/harmonyos/compiler/ninja/1.9.0/linux/ninja.1.9.0.tar \
	&& wget -P /home/tools https://repo.huaweicloud.com/harmonyos/compiler/gn/1717/linux/gn-linux-x86-1717.tar.gz \
	&& wget -P /home/tools https://mirrors.huaweicloud.com/nodejs/v14.15.4/node-v14.15.4-linux-x64.tar.xz \
	&& wget -P /home/tools https://hm-verify.obs.cn-north-4.myhuaweicloud.com/qemu-5.2.0.tar.xz \
	&& locale-gen "en_US.UTF-8" \
	&& rm -rf /bin/sh /usr/bin/python /usr/bin/python3 \
	&& ln -s /bin/bash /bin/sh \
	&& ln -s /usr/bin/python3.8 /usr/bin/python3 \
	&& ln -s /usr/bin/python3.8 /usr/bin/python \
	&& tar -xvf /home/tools/llvm.tar.gz -C /home/tools \
	&& tar -xvf /home/tools/hc-gen-0.65-linux.tar -C /home/tools \
	&& tar -xvf /home/tools/gcc_riscv32-linux-7.3.0.tar.gz -C /home/tools \
	&& tar -xvf /home/tools/ninja.1.9.0.tar -C /home/tools \
	&& tar -xvf /home/tools/gn-linux-x86-1717.tar.gz -C /home/tools/gn \
	&& tar -xJf /home/tools/node-v14.15.4-linux-x64.tar.xz -C /home/tools \
	&& cp /home/tools/node-v14.15.4-linux-x64/bin/node /usr/local/bin \
	&& ln -s /home/tools/node-v14.15.4-linux-x64/lib/node_modules/npm/bin/npm-cli.js /usr/local/bin/npm \
	&& tar -xJf /home/tools/qemu-5.2.0.tar.xz -C /home/tools \
	&& sed -i '$aexport PATH=/home/tools/llvm/bin:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/hc-gen:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/gcc_riscv32/bin:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/ninja:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/gn:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/home/tools/node-v14.15.4-linux-x64/bin:$PATH' /root/.bashrc \
	&& sed -i '$aexport PATH=/root/.local/bin:$PATH' /root/.bashrc \
	&& export PATH=/home/tools/llvm/bin:$PATH \
	&& export PATH=/home/tools/hc-gen:$PATH \
	&& export PATH=/home/tools/gcc_riscv32/bin:$PATH \
	&& export PATH=/home/tools/ninja:$PATH \
	&& export PATH=/home/tools/node-v14.15.4-linux-x64/bin:$PATH \
	&& export PATH=/home/tools/gn:$PATH \
	&& export PATH=/root/.local/bin:$PATH \
	&& cd /home/tools/qemu-5.2.0 \
	&& mkdir build \
	&& cd build \
	&& ../configure --target-list=arm-softmmu \
	&& make -j \
	&& make install \
	&& cd /home/openharmony \
	&& rm -rf /home/tools/*.xz \
	&& rm -rf /home/tools/*.gz \
	&& rm -rf /home/tools/*.tar \
	&& rm -rf /home/tools/qemu-5.2.0 \
	&& npm install -g @ohos/hpm-cli --registry https://mirrors.huaweicloud.com/repository/npm/

ENV LANG=en_US.UTF-8 LANGUAGE=en_US.UTF-8 LC_ALL=en_US.UTF-8 TZ=Europe/Moscow
