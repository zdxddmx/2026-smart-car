# AGENTS.md

本文件为 ZCode 代理在本工程中工作提供项目专属指引。代码注释与提交信息以中文为主。

## 工程概述

智能车竞赛电磁组固件，运行于 **STC32G（AI32G12K128）** 32 位 8051 内核单片机，主频 30MHz（`SYSTEM_CLOCK_30M`）。
功能：4 路电磁电感循迹、IMU660RA 陀螺仪姿态积分、DRV8701 轮驱电机、负压电机、带方向编码器测速/测距、环岛状态机导航。

## 目录结构

- `Libraries/` — 逐飞科技（SeekFree）STC32G 开源库 V3.1.7（GPL3.0）。**视为只读**，除非有充分理由不要修改。
  - `zf_common/` 通用层（clock/typedef/interrupt/headfile 等）
  - `zf_driver/` 外设驱动（adc/gpio/pwm/uart/timer/encoder/pit 等）
  - `zf_device/` 设备驱动（imu660ra/rb、ips114、wireless_uart 等）
  - `zf_components/` seekfree_assistant 组件
  - `doc/version.txt` 库版本与变更说明
- `Project/code/` — **应用代码，主要改动区域**
  - `Common_peripherals.c/.h` 外设封装与引脚宏（蜂鸣器/按键/DRV8701/负压电机/编码器）
  - `Adc.c/.h` ADC 采集、中位值滤波、归一化、差比和偏差计算
  - `Imu.c/.h` IMU660RA 角速度读取与角度积分（YAW/PITCH）
  - `Agorithm.c/.h` PID 算法（方向环 `place_pid`、速度环 `l/r_speed_pid`、`LowPassFilter`）
  - `Body_ctrl.c/.h` 导航状态机（环岛识别 `State_scan` / `State_execution`，`A_navigation`）
  - `My_isr.c/.h` 定时器中断封装（`pit_0/pit_1`）
- `Project/user/` — `main.c`（入口、外设初始化、主循环）、`isr.c/.h`（底层中断向量服务函数）
- `Project/mdk/` — Keil 工程文件（`seekfree.uvproj`）、`MDK删除临时文件.bat`（清理构建产物）

## 构建与烧录

- **工具链**：Keil MDK **FOR C251**（C251 编译器，非 ARM）。打开 `Project/mdk/seekfree.uvproj` 构建。
- **优化等级必须为 O0**（见 `Libraries/doc/version.txt`：更高等级存在编译器 bug）。当前工程 `Optim=0`。
- 构建产物输出到 `Project/mdk/out_file/`（已 gitignore）。
- 清理：运行 `Project/mdk/MDK删除临时文件.bat`（删 `out_file/` 及 `.uvopt`/`.uvgui.*`）。
- 无命令行 lint/typecheck/test；验证方式为烧录上车主控实测。

## 架构与分层规则

- **统一头文件**：所有 `.c` 文件首行 `#include "zf_common_headfile.h"`（位于 `Libraries/zf_common/`）。该文件集中 include 全部库头文件与 `Project/code/` 下各模块头文件。新增模块头文件需在此处追加 include，并相应在 Keil 工程加入源文件。
- **中断调用链**：`isr.c` 中的 `TM1_IRQHandler` 等向量调用函数指针 `tim1_irq_handler` → 由 `My_isr.c` 的 `pit_1_ms_init` 注册为 `pit_1_handler`。**周期性任务一律在 `pit_1_handler` 内调用**（IMU/编码器/ADC 采样、导航、急停检测），不要塞进 `main` 的 `while(1)`。主循环与定时器中断严格分离（见 `日志.txt` 2026/4/12 说明）。
- **TIM1 优先级默认最低且不可修改**（STC 手册限制，见 `My_isr.c` 注释）；TIM0 优先级可设。
- **库层与应用层边界**：应用代码不直接改 `Libraries/`；如需启用/禁用设备驱动，在 `zf_common_headfile.h` 注释/解开对应 `#include`，并在 `zf_device_config.h` 配置宏。

## 编码约定

- 命名：小写+下划线，模块前缀（`Buzzer_init`、`DRV8701_D_motor_ctrl`、`Imu_value_get`）。测试函数以 `_text` 结尾（`ADC_text`、`Imu_text`、`Encoder_text`）。
- 引脚与常量用 `#define` 宏（`#define Buzzer_pin IO_P67`），集中在 `Common_peripherals.h`。
- 模块间共享状态用全局变量 + 头文件 `extern`（`LA_SPEED`、`Track_error`、`key1_flag`、`HD_flag`、`Imu_open_flag` 等）。
- 限幅多用三目嵌套内联（`x>M_MAX?M_MAX:(x<M_MIN)?M_MIN:x`）。
- PID 输出做一阶低通（系数 `A=0.9`），IMU 角速度积分前有死区滤波。
- 中文注释，保留逐飞库文件的版权声明头部。

## 关键状态与注意事项

- `top`（main.c 中 `extern`）：无线串口收到 `0x05` 置 1 → `pit_1_handler` 内进入急停死循环（电机停转、蜂鸣器长响）。改急停逻辑时注意该循环不可退出。
- `HD_flag`：距离积分开关（环岛预判时置 1，入环时清 0）；`Distance` 累积编码器均值。
- `Imu_open_flag`：角度积分开关（仅入环岛阶段置 1）。
- 负压电机当前启用 `NEG_motor_ctrl(speed, step)`（带斜坡），`FY_pin = PWMB_CH3_P33`，17kHz。`bldc_set_speed`（50Hz 无刷电调）已注释保留，切换时需同步改 `FY_init` 频率与 `Common_peripherals.h` 宏。
- `pit_1_handler` 内每 10ms 翻转 `IO_P52`（LED），用于判断中断是否卡死——若 LED 停闪，说明中断被阻塞。
- ADC 归一化极值 `adc_min[4]`/`adc_max[4]` 需按实际标定（`Adc.c` 顶部）。
- `zf_common_headfile.h` 已 `#pragma warning disable = 115, 188`，不要因这些告警改动库。

## 文档参考

改敏感区域前先读：
- `Project/code/日志.txt` — 历次变更与设计决策（中断重构原因、调试方法）。
- `Libraries/doc/version.txt` — 库版本与已知 bug（优化等级、引脚错误修复等）。
