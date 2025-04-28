# HPM案例发行版制作与发布流程说明

## 准备<a name="summary"></a>

- 编译环境：Linux | Ubuntu
- [开发案例](https://growing.openharmony.cn/mainPlay/sample)
- HPM注册及权限获取[官网]([DevEco Marketplace (harmonyos.com)](https://hpm.harmonyos.com/#/cn/help/introduction))

  1.注册账号实名认证后登录，进行[个人设置](https://hpm.harmonyos.com/#/cn/help/personalset)
  ![](imgs/05/1-%E5%87%86%E5%A4%8701.png)
  2.进入个人中心
  ![](imgs/05/1-%E5%87%86%E5%A4%8702.png)

  3.左侧菜单栏选择组织(组织创建请参考[组织管理](https://repo.harmonyos.com/#/cn/help/orgmanage))，右侧列表中选择您所在的组织，点击“操作”下图标进入详情页
  ![](imgs/05/1-%E5%87%86%E5%A4%8703.png)

## HPM基础开源发行版<a name="distribution"></a>

| 开源发行版名称    |      组件包名称       | 芯片类型    | OS系统版本-> 组件包版本                         |
| :---------------- | :-------------------: | ----------- | :---------------------------------------------- |
| WLAN连接类产品    | @ohos/hispark_pegasus | Hi3861      | 1.0.1 -> 1.1.4、3.0-LTS -> 3.0.0、 3.1 -> 3.1.1 |
| 摄像头+屏幕类产品 | @ohos/hispark_taurus  | Hi3516DV300 | 同上                                            |
| 摄像头类产品      |  @ohos/hispark_aries  | Hi3518EV300 | 同上                                            |

[更多开源发行版查看]([Hi3861](https://repo.harmonyos.com/?#/cn/solution))

## 开源发行版制作流程<a name="steps"></a>

此说明手册以 **润和 智能炫彩灯** 为例进行操作。[案例地址](https://growing.openharmony.cn/mainPlay/detail?sampleId=3787)

![](imgs/05/2-%E6%B6%A6%E5%92%8C%E6%99%BA%E8%83%BD%E7%82%AB%E5%BD%A9%E7%81%AF.png)

#### 1、选择基础开源发行版
- 发行版名称：根据 [HPM基础开源发行版](#distribution) 查询 “Pegasus智能家居套件”的发行版名称为 “WLAN连接类产品”，组件包名称为 @ohos/hispark_pegasus
- 发行版版本：根据 [HPM基础开源发行版](#distribution) 查询 OS系统版本3.0-LTS组件包版本为3.0.0

  **润和 智能炫彩灯** 的基础发行版为 @ohos/hispark_pegasus@3.0.0

  **其它系统版本** ：根据系统版本的分支信息去码云上 [OpenHarmony](https://gitee.com/openharmony/docs/tree/master/zh-cn/release-notes)项目查询对应的标签号，再根据标签号查询HPM上发行版的版本，并选择合适的发行版版本使用即可。

#### 2、下载并安装基础发行版

```
// 发行版：@ohos/hispark_pegasus，此手册制作使用版本：3.0.0，命令如下
mkdir hpmDistDemo
cd hpmDistDemo
hpm init -t dist
hpm install @ohos/hispark_pegasus@3.0.0
```

下载完成后，即可根据手册步骤进行案例代码所需要的修改操作。
- 进入目录```hpmDistDemo/ohos_bundles/@ohos/hispark_pegasus```
- 修改scripts/dist.sh制定编译具体的要发行的案例，使用```hpm dist```命令代替```hb set```以及```hb build```命令，dish.sh基本信息如下：*（说明：此处wifiiot_hispark_pegasus@hisilicon为发行案例编译的版本，不同案例编译版本有所不同）*

```
#!/bin/bash
cd $DEP_BUNDLE_BASE
if [ ! -f build.py ]; then
	ln -s build/lite/build.py build.py
fi
cd -
python3 $DEP_BUNDLE_BASE/build.py -f -p wifiiot_hispark_pegasus@hisilicon
```

[关于环境变量说明查看]([HPM Part参考 | OpenHarmony](https://docs.openharmony.cn/pages/zh-cn/device/%E8%AE%BE%E5%A4%87%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3/%E4%B8%93%E9%A2%98/HPM%20Part/HPM%20Part%E5%8F%82%E8%80%83/#%E5%85%B3%E4%BA%8E%E7%8E%AF%E5%A2%83%E5%8F%98%E9%87%8F))

#### 3、修改代码并生成依赖包

[智能炫彩灯](https://gitee.com/openharmony-sig/knowledge_demo_temp/tree/master/docs/hi3861_v100_demo_Smartbrightlights)

![](imgs/05/3-%E4%BF%AE%E6%94%B9%E4%BB%A3%E7%A0%81.jpg)

[Demo源码下载](https://gitee.com/openharmony-sig/knowledge_demo_temp/tree/master/dev/team_x/demo_Smartbrightlights)

- 如图，修改涉及的代码：
    ```applications/sample/wifi-iot/app/iothardware```
    ```applications/sample/wifi-iot/app/BUILD.gn```
    与发行版基础依赖（**@ohos/applications_sample_wifi_iot**）代码目录("destPath"字段)一致，故而需要重新制作对应的依赖来替换上面涉及的依赖，如下所示：
```
// bundle.json位置：hpmDistDemo/applications/sample/wifi-iot/bundle.json
{
    "name": "@组织名称/applications_sample_wifi_iot_brightlights",
    "version": "3.0.0",
    "publishAs": "code-segment",
    "description": "Samples of wifi_iot",
    "scripts": {
	    "install": "DEST_PATH=$DEP_BUNDLE_BASE/applications/sample/wifi-iot && mkdir -p $DEST_PATH && cp -r . $DEST_PATH"
    },
    "tags": [
        "applications"
    ],
    "ohos": {
        "os": "3.0.0",
        "kernel": "liteos-m",
        "board": "hi3861"
    },
    "license": "Apache V2",
    "keywords": [
        "applications", "sample", "wifi", "iot"
    ],
    "segment": {
	"destPath": "applications/sample/wifi-iot"
    },
    "repository": "",
}
```
此涉及变更依赖组件：
@ohos/applications_sample_wifi_iot 修改为 @组织名称/applications_sample_wifi_iot_brilights

- 修改 device/hisilicon/hispark_pegasus/sdk_liteos/build/config/usr_config.mk

```
...
# CONFIG_PWM_SUPPORT is not set
```

修改为：

```
...
CONFIG_PWM_SUPPORT=y
```

此涉及变更依赖组件：
@ohos/device_hisilicon_hispark_pegasus 修改为 @组织名称/device_pwm

#### 4、案例发行版编译测试

```
cd ~/hpmDistDemo
hpm dist

# 回车，等待编译成功
# 编译成功后产物在 ~/hpmDistDemo/out目录，后续进行烧录验证
# 验证通过，最后检查发行版、依赖（包括bundle.json、README.md、LICENSE等）没问题即可发布至HPM官网
```

#### 5、发布至HPM相关操作

- 若有修改需要重新发布新的依赖组件，如
```
cd hpmDistDemo/applications/sample/wifi-iot
hpm publish

# 回车，等待发布完成
```

- 发布案例发行版，发布前还需要修改基础发行版的bundle.json，如：案例名称为 **@组织名称/发行版名称**，还需要对应修改发行版版本号、代码仓库、代码目录、依赖等，【**特别提醒**】如有修改依赖请记得修改或添加```dependencies```中的依赖组件。

```
// bundle.json位置：hpmDistDemo/ohos_bundles/@ohos/hispark_pegasus/bundle.json，此案例做如下修改
{
    "name": "@组织名称/hoperun_hm_brightlights",
    "version": "3.0.0",
    "publishAs": "distribution",
    "description": "样例演示如何在Pegasus Wi-Fi IoT智能家居套件上使用PWM实现炫彩灯",
    "license": "Apache V2",
    "scripts": {
        "config_hb": "hb set -root $DEP_BUNDLE_BASE",
        "dist": "PATH=/root/.local/bin:${DEP_OHOS_gn}:${DEP_OHOS_ninja}/ninja:${DEP_OHOS_llvm}/llvm/bin:${DEP_OHOS_gcc_riscv32}/gcc_riscv32/bin:${PATH} && chmod +x ./scripts/dist.sh && ./scripts/dist.sh"
    },
    "envs": {
        "debug": true
    },
    "dirs": {
        "scripts": "scripts/*"
    },
    "dependencies": {
        "@ohos/build_lite": "3.0.0",
        "@ohos/distributeddatamgr_appdatamgr": "3.0.0",
        ...
    },
    "ohos": {
        "os": "3.0.0",
        "board": "Hi3861",
        "kernel": "liteos-m"
    },
    "keywords": [
        "hispark", "pegasus",
        ...
    ],
    "repository": "https://gitee.com/openharmony-sig/knowledge_demo_temp/tree/master/docs/hi3861_v100_demo_Smartbrightlights"
}
```

- 发布前，需要在README.md文件中增加对开发的发行版功能特性、使用方法等信息进行描述

- 同时开源使用声明定义并生成LICENSE文件，此案例使用```Apache V2```开源协议

```
cd ~/hpmDistDemo
mkdir publish
cp -rp ohos_bundles/@ohos/hispark_pegasus/* ./publish
cd publish
hpm publish

# 回车，等待发布完成
```

#### 6、生成开源发行版

- 选择“包”Tab，过滤下拉框选择“distribution”选项，查询当前已通过的发行版组件包
![](imgs/05/4-%E7%94%9F%E6%88%90%E5%BC%80%E6%BA%90%E5%8F%91%E8%A1%8C%E7%89%8801.jpg)

- 点击“操作”下图标按钮后弹出输入框，填写相关信息(图片支持png、jpg、gif 格式)，点击“提交”按钮[生成开源发行版]([DevEco Marketplace (harmonyos.com)](https://repo.harmonyos.com/#/cn/help/contributedistribution))申请，待管理员审核通过即可发布至HPM开源发行版页面
![](imgs/05/4-%E7%94%9F%E6%88%90%E5%BC%80%E6%BA%90%E5%8F%91%E8%A1%8C%E7%89%8802.png)