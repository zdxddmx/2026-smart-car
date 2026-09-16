# 第三方组件说明

本仓库包含以下第三方代码。它们的版权与许可归各自作者所有，**不适用**根目录 `LICENSE` 中的条款。

## 逐飞科技 STC32G 开源库 V3.1.7

位置：`Libraries/`

Copyright (c) 2022 SEEKFREE 成都逐飞科技有限公司

按 GNU General Public License v3.0 分发。许可证正文见 `Libraries/doc/LICENSE`，根目录 `LICENSE` 为同一份文本。各源文件头部的版权声明请勿删改。

本固件静态链接了该库，因此整个工程按 GPL-3.0 分发。

## Keil C251 启动代码

位置：`Libraries/zf_common/START251.A51`

Copyright KEIL ELEKTRONIK GmbH 1995 - 2000。随 Keil MDK FOR C251 工具链提供，按 Keil 的许可条款使用。

## 关于文件编码

`Libraries/` 与 `Project/` 下的源文件原本为 GBK 编码，现已统一转为 UTF-8（无 BOM）。这只是显示与可移植性的调整，未改动任何代码逻辑。依据 GPL-3.0 第 5 条，在此声明该修改。

选 UTF-8 还有一个实际原因：GBK 双字节的第二字节可能是 `0x5C`（反斜杠），中文注释位于 `//` 行尾时会吞掉换行符，在 Keil C251 上是常见坑。UTF-8 的续字节范围是 `0x80–0xBF`，不存在这个问题。
