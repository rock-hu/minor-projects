# OH_AI_ShapeInfo
<!--Kit_MindSpore Lite Kit--><!--System_AI-->

## 概述

形状维度大小，预留最大维度是32，当前实际支持的最大维度是8。

**起始版本：** 9

**相关模块：** [MindSpore](capi-mindspore.md)

**所在头文件：** [model.h](capi-model-h.md)

## 汇总

### 成员变量

| 名称 | 描述 |
| -- | -- |
| size_t shape_num | 维度数组长度。 |
| int64_t shape[OH_AI_MAX_SHAPE_NUM] | 维度数组。 |