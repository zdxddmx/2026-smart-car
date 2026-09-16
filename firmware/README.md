# 2026 智能汽车竞赛 · 电磁组车载固件（STC32G）

跑在 STC32G（AI32G12K128）上的电磁组循迹小车固件。四路电感采集赛道磁场，差比和算出横向偏差，方向环用 PD 加陀螺仪阻尼控制转向，速度环做增量式 PI，编码器积分提供环岛判据，环岛进出交给一个独立状态机。工具链是 Keil MDK FOR C251，不是 ARM。

配套硬件（DRV8701 驱动板、主板）在 [smartcar-hardware](https://github.com/zdxddmx/smartcar-hardware)。

## 目录

```
Libraries/          逐飞科技 STC32G 开源库 V3.1.7（GPL3.0，视为只读）
  zf_common/          通用层：时钟、类型、中断、统一头文件
  zf_driver/          外设驱动：adc gpio pwm uart timer encoder pit ...
  zf_device/          设备驱动：imu660ra ips114 wireless_uart ...
  zf_components/      seekfree_assistant 上位机组件
Project/code/       应用代码，主要改动区域
  Common_peripherals.c/.h   蜂鸣器、按键、DRV8701、负压电调、编码器
  Adc.c/.h                  电感采集、滤波、归一化、差比和偏差
  Imu.c/.h                  IMU660RA 角速度读取与 YAW/PITCH 积分
  Agorithm.c/.h             PID（方向环、双速度环）与一阶低通
  Body_ctrl.c/.h            导航状态机与环岛识别
  My_isr.c/.h               定时器中断封装
Project/user/       main.c 入口、isr.c 中断向量
Project/mdk/        Keil 工程 seekfree.uvproj
AGENTS.md           工程导览：分层规则、编码约定、注意事项
Project/code/日志.txt  历次变更与设计决策
```

## 控制结构

采样到执行是一条固定链路：`pit_1_handler` 每 5 ms 跑一次，顺序取 IMU 角速度、编码器计数、四路电感 ADC，然后把结果交给导航状态机；状态机决定走普通循迹还是环岛逻辑，最后落到 `DRV8701_D_motor_ctrl` 输出两路 PWM。

中断采用回调注册而不是直接在向量里写逻辑。`isr.c` 的 `TM1_IRQHandler` 只调用函数指针 `tim1_irq_handler`，由 `My_isr.c` 的 `pit_1_ms_init` 把它注册成 `pit_1_handler`。所有周期性任务都在 `pit_1_handler` 内部，主循环只处理无线串口接收，两边不互相干扰。这个拆分是 2026/4/12 那版改的，之前编码器和定时器抢资源导致计数错乱。

`pit_1_handler` 里还有一行翻转 `IO_P52` 的代码，每 10 个中断周期翻一次。LED 停闪就说明中断被卡住了，这是车上最直接的自检手段。

方向环 `place_pid` 是 PD 加陀螺仪阻尼，输出再做一阶低通（系数 0.9 对新值加权）：

```
place_out = kp * err + kd * (err - last_err) + gyro_kd * imu660ra_gyro_z
place_out = place_out * 0.9 + place_last_out * 0.1
```

速度环是增量式 PI，左右两路各自把方向环输出叠加到基础速度上，符号相反：

```
左轮 aim = base_speed - place_out
右轮 aim = base_speed + place_out
```

环岛识别靠电感总值的左右对比。`R_ALL > L_ALL && ALL_DG >= 210` 判右环，`L_ALL > R_ALL && ALL_DG >= 230` 判左环，同时把 `HD_flag` 置 1 开始累积编码器距离；距离到 15558 触发入环转角，转角阶段用陀螺仪积分角度做闭环，转到目标角度后归零回到正常循迹。

## 引脚分配

| 功能 | 引脚 |
| --- | --- |
| 蜂鸣器 | `IO_P67` |
| 按键 KEY1–KEY4 | `IO_P70` `IO_P71` `IO_P72` `IO_P73` |
| 拨码 Switch1 / Switch2 | `IO_P75` / `IO_P76` |
| 左电机 DIR / PWM | `IO_P60` / `PWMA_CH2P_P62` |
| 右电机 DIR / PWM | `IO_P64` / `PWMA_CH4P_P66` |
| 负压电调 PWM | `PWMB_CH4_P77` |
| 编码器 1 DIR / PULSE | `IO_P35` / `TIM0_ENCOEDER_P34` |
| 编码器 2 DIR / PULSE | `IO_P53` / `TIM3_ENCOEDER_P04` |
| 中断自检 LED | `IO_P52` |
| 电感 L_H / L_S / R_S / R_H | `ADC_CH8_P00` / `ADC_CH9_P01` / `ADC_CH13_P05` / `ADC_CH14_P06` |

轮驱 PWM 频率 17 kHz，占空比限幅 ±8000。负压电调走 50 Hz 标准油门，`bldc_set_speed(percent)` 把 0–100 映射到 duty 500–1000。

## 当前调试状态

这一节比上面都重要。仓库当前 HEAD 把大部分执行机构调用点注释掉了，烧进去车不会跑，只会采样和响应急停。要恢复整机运行得按顺序打开：

| 位置 | 被注释的内容 | 打开后的效果 |
| --- | --- | --- |
| `My_isr.c` | `A_navigation()` | 导航状态机开始工作，才会调用 `Normal_run` / `Ins_run` |
| `My_isr.c` | `l_speed_pid` / `r_speed_pid` 与对应 `DRV8701_D_motor_ctrl` | 速度环闭环接管轮速 |
| `My_isr.c` | `bldc_set_speed(90)` / `bldc_set_speed(35)` | 负压电机按俯仰角自动切换转速 |
| `main.c` | `while(1)` 里的测试函数调用 | `Imu_text` / `ADC_text` / `Encoder_text` 上屏 |

唯一处于活动状态的执行代码是无线串口急停：主循环收到 `0x05` 把 `top` 置 1，`pit_1_handler` 检测到后回发 `0x08`、关掉两个轮驱和负压、蜂鸣器长响，然后进入死循环不再退出。改急停逻辑时注意这个循环是故意不可退的。

普通循迹走的是开环 PWM（`1500 ± place_out`），`base_speed` 在 HEAD 里是 0，速度环参数虽然写好了但还没并进去。这是标定过程中的中间状态，不是最终形态。

## 电感标定

`Adc.c` 顶部的 `adc_min[4]` 和 `adc_max[4]` 必须按实车重新标定，仓库里那组数只是某一版的值。标定方法（2026/2/26 日志）：

横放电感，把电感垂直正对电磁线，此时读数记为 MAX，与它配对的那个电感读数记为 MIN。竖放电感找个直角位置取值。四路都单独处理，不要共用一组极值。

采到的原始值先做中位值平均滤波（去掉一个最大一个最小再平均），再归一化到 1–100，最后进差比和：

```
sub = (L_H - R_H) + (L_S - R_S)
add = (L_H + R_H) + |L_S - R_S| + 1
Track_error = sub * 100 / add
```

## 构建与烧录

1. 用 Keil MDK **FOR C251** 打开 `Project/mdk/seekfree.uvproj`。注意是 C251 不是 ARM 版 MDK，装错了打不开。
2. **优化等级必须是 O0**。逐飞库的 `doc/version.txt` 记录了更高优化等级会踩编译器 bug，工程里 `Optim=0` 已经是正确值，别改。
3. 编译产物输出到 `Project/mdk/out_file/`，生成 `SEEKFREE.hex`，该目录已被 gitignore。
4. 清理临时文件跑 `Project/mdk/MDK删除临时文件.bat`，它会删掉 `out_file/` 和 `.uvopt` / `.uvgui.*`。

工程没有任何命令行 lint、typecheck 或单元测试，验证方式就是烧到车上实测。

## 版本记录

```
a0c9b63  feat: 启用无刷负压电机与整车控制初始化
b1bfa32  chore: 切换负压电机驱动并新增 AGENTS.md 指引
e3d0685  feat: 切换无刷负压电机驱动并重新标定 PID 与电感参数
9d03904  版本 1.1.2: 修正电机转向方向，调整方向环 PID 参数
785e241  feat: 调整 PID 参数并启用导航与负压电机控制
c168379  fix: 调整 ADC 极值与 IMU 滤波参数并切换测试函数
ac04177  chore: 切换至 ADC 测试模式并更新屏幕显示
cca1117  版本 1.1.1: 修正电机转向配置，临时启用轮驱电机测试
4a0b205  版本 1.1.0: 加入无线串口急停，调整负压电机启动配置
12d23fe  版本 1.0.0: STC32G 平衡车工程首版入库
```

## 许可

`Project/`、`AGENTS.md` 和 `Project/code/日志.txt` 是本队的应用代码。

`Libraries/` 是成都逐飞科技有限公司的 STC32G 开源库 V3.1.7，按 GPL3.0 分发，版权与许可声明保留在各源文件头部，不要删改。

仓库目前没有附带 GPL3.0 许可证正文。`Libraries/doc/` 下只有 `version.txt`，库文件头部指向的 `libraries/doc/LICENSE` 和 `GPL3_permission_statement.txt` 都不在。GPL3 要求分发时随附许可证文本，补上之后再对外发布比较稳妥。
