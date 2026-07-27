# Docker core

### 镜像简介

本文为Docker core编译环境使用指导。

OpenHarmony的Docker镜像托管在[HUAWEI Cloud SWR](https://auth.huaweicloud.com/authui/login.html?service=https%3A%2F%2Fconsole.huaweicloud.com%2Fswr%2F%3Fregion%3Dcn-south-1%26cloud_route_state%3D%2Fapp%2Fwarehouse%2FwarehouseMangeDetail%2Fgoldensir%2Fopenharmony-docker%2Fopenharmony-docker%3Ftype%3DownImage&locale=en-us#/login)上，开发者可以通过该镜像在很大程度上简化编译前的环境配置。 目前容器化构建选项支持情况如下：

| Docker镜像仓库                                               | 标签    | 说明                                  |
| :----------------------------------------------------------- | :------ | :------------------------------------ |
| `swr.cn-south-1.myhuaweicloud.com/openharmony-docker/docker_oh_mini` | `3.2` | 预安装OpenHarmony版本L0的编译环境。 |
| `swr.cn-south-1.myhuaweicloud.com/openharmony-docker/docker_oh_small` | `3.2` | 预安装OpenHarmony版本L0&L1的编译环境。 |
| `swr.cn-south-1.myhuaweicloud.com/openharmony-docker/docker_oh_standard` | `3.2` | 预安装OpenHarmony版本L2的编译环境。 |
| `swr.cn-south-1.myhuaweicloud.com/openharmony-docker/docker_oh_full` | `3.2` | 预安装OpenHarmony版本L0&L1&L2的编译环境。 |

**说明：下文中的`${dockerType}`指代不同类型的Docker core镜像（包括mini、small、standard、full）,`${version}`指代的是发布的Docker core标签版本**

### Docker core 使用方式及步骤

 开发者在下载好源码后，可以通过以下步骤来使用我们提供的Docker环境。**需要使用静态编译方式以规避使用GPL协议的工具源问题**

1. 获取Docker镜像

     ```
     docker pull swr.cn-south-1.myhuaweicloud.com/openharmony-docker/docker_oh_${dockerType}:${version}
     ```

2. 进入OpenHarmony代码根目录执行如下命令进入Docker构建环境，Linux环境下执行

     ```
     docker run -it -v $(pwd):/home/openharmony swr.cn-south-1.myhuaweicloud.com/openharmony-docker/docker_oh_${dockerType}:${version}
     ```
     Windows下执行（假设源码目录在 D:\OpenHarmony）:
     ```
     docker run -it -v D:\OpenHarmony:/home/openharmony swr.cn-south-1.myhuaweicloud.com/openharmony-docker/docker_oh_${dockerType}:${version}
     ```

3. 通过如下命令启动不同平台的编译

     ```
     # 若以repo下载的源码
     hb set #在显示的页面中通过键盘上下键选择需要编译的平台，通过回车确定选择。
     hb build -f #执行编译。
     ```
     ```
     # 若以HPM方式获取的基础发行版
     hpm dist
     ```

### 基于Docker core/Dockerfile贡献

1. 基于发布的Docker core生成新的Dockerfile

    ```
    FROM swr.cn-south-1.myhuaweicloud.com/openharmony-docker/docker_oh_${dockerType}:${version}
    WORKDIR /home/openharmony
    RUN wget ...
    ```

2. 基于core Dockerfile生成新的Dockerfile

    ```
    # 下载Gitee仓库中的Dockerfile，直接添加编译工具的脚本，下载地址：
    https://gitee.com/openharmony-sig/cicd/docker/Docker_OH_${dockerType}/Dockerfile

    ```

### Docker core中工具版本选择原则

1.系统工具跟随镜像源最新版本

2.非系统工具取  [华为云](https://repo.huaweicloud.com/harmonyos/compiler/) 上对应工具版本的二进制，Docker core安装使用；华为云无法获取对应工具版本的二进制，Docker core安装指定官方版本

