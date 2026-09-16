# 2026 全国大学生智能汽车竞赛 · 电磁组

一辆电磁循迹小车的完整工程。板子自己画，代码自己写，硬件设计和车载固件放在同一个仓库里。

```
firmware/   STC32G 车载固件（Keil MDK FOR C251）
hardware/   嘉立创 EDA 板卡源文件
```

## 整车构成

主控是 STC32G（AI32G12K128），8051 内核，跑在 30 MHz。四路电感采赛道磁场，经运放调理后进 ADC；差比和算出横向偏差，方向环用 PD 加陀螺仪阻尼控制转向，速度环做增量式 PI。轮驱走 DRV8701，带方向编码器负责测速和距离积分，环岛进出交给一个独立状态机。无刷负压电机把车压在地面上减少打滑。

## 硬件与固件的对应关系

硬件和固件不是两堆无关的文件，板子上的每一路信号在固件里都有对应的引脚宏和标定参数。改动硬件通常要同步改固件：

| 板卡 | 车上的角色 | 固件里的对应位置 |
| --- | --- | --- |
| `ProPrj_STC电磁2.0挖孔.epro` | 主控板，挖孔轻量化 | 整个 `firmware/Project/` |
| `ProPrj_DRV8701电机双驱.epro` 等三块 | 轮驱功率级 | `Common_peripherals.c` 的 `DRV8701_init` / `DRV8701_D_motor_ctrl` |
| `ProPrj_4路分体电磁循迹模块.epro` | 四路电感采集 | `Adc.c` 的四路 `adc_init` 与归一化极值 |
| `ProPrj_OPA4377运放改良.epro` | 电感信号调理，提升信噪比 | 决定 `Adc.c` 顶部 `adc_min[4]` / `adc_max[4]` 的标定范围 |

运放那块板子的改良直接影响归一化区间，换板子之后 `adc_min` / `adc_max` 必须重新标定，否则偏差计算会整体跑偏。

## 硬件

六块自研板卡，全部用嘉立创 EDA 完成原理图与 PCB 并打样焊接。`.epro` 文件用嘉立创 EDA（专业版）直接双击打开。

DRV8701 双驱有两个版本，`DRV8701E双驱，曙光` 是改进布局版；单驱方案保留用于对照。循迹模块做成分体式，方便单独更换和调间距。

仓库只含设计源文件，不含 Gerber、BOM 与打样文件。详见 [`hardware/README.md`](hardware/README.md)。

## 固件

`firmware/` 下是完整的 Keil 工程。分层是逐飞科技库（`Libraries/`）+ 应用代码（`Project/code/`）+ 入口与中断向量（`Project/user/`）。

采样到执行走一条固定链路：`pit_1_handler` 每 5 ms 跑一次，顺序取 IMU 角速度、编码器计数、四路电感 ADC，交给导航状态机决定走普通循迹还是环岛逻辑，最后落到 `DRV8701_D_motor_ctrl` 输出两路 PWM。主循环只处理无线串口接收，和定时器中断严格分开。

需要留意的是，当前分支把大部分执行机构调用点注释掉了，烧进去车不会跑，只会采样和响应急停。哪些开关处于什么状态、怎么逐层打开、引脚分配表、电感标定方法、构建与烧录步骤，都写在 [`firmware/README.md`](firmware/README.md) 里。

工程导览和编码约定在 [`firmware/AGENTS.md`](firmware/AGENTS.md)。工具链是 Keil MDK **FOR C251**（不是 ARM 版），优化等级必须为 O0，逐飞库在更高优化等级下有编译器 bug。

## 许可

固件静态链接了逐飞科技的 GPL-3.0 开源库，所以 `firmware/` 按 **GPL-3.0** 分发，根目录的 `LICENSE` 即该文本，逐飞库原件另有副本在 `firmware/Libraries/doc/LICENSE`。

`hardware/` 下是自己画的板卡设计，按 **MIT** 分发，见 `hardware/LICENSE`。

第三方组件（逐飞库、Keil 启动代码等）的版权归属单独列在 [`firmware/THIRD_PARTY_NOTICES.md`](firmware/THIRD_PARTY_NOTICES.md)。
