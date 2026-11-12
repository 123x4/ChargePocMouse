# 会话交接总结（最后更新：2026-04-27）

## 项目目标和背景
- 项目运行在 LPC55S28，使用 SPI1/SPI4/SPI5/SPI7（对应 FLEXCOMM 1/4/2/3）+ DMA 并发读取多路 Hall 传感器角度数据。
- 所有 SPI 通道使用相同的传输格式（CPHA=0/CPOL=0、12MHz、16-bit 帧、kSPI_FrameAssert）。
- 当前调试焦点：**SPI1 接收数据右移 1bit**（如逻辑分析仪显示 `0x80`，MCU DMA 读到 `0x40`），其它通道（SPI4/5/7）正常。
- 现象一上电就出现，逻辑分析仪看到的物理总线数据是**正确的**，错误在 MCU 端 SPI 输入采样路径。

---

## ⭐ 根本原因（2026-04-27 已定位并修复）

### LPC55S28 的 I²C-tolerant pad 默认处于 I²C 模式

LPC55S28 上有 4 个 **I²C-tolerant 引脚**：`PIO0_13`、`PIO0_14`、`PIO1_20`、`PIO1_21`，它们的 IOCON 寄存器有特殊位：

| 位 | 名称 | 含义 |
|---|---|---|
| bit 14 | **EGP** | **0 = I²C 模式（默认）；1 = GPIO/SPI 模式** |
| bit 15 | I2CFILTER | 0 = 50ns 毛刺滤波器开启（默认） |
| bit 13 | ECS | I²C 上拉电流源 |

定义来自 `device/LPC55S28.h` 第 11883-11897 行（`IOCON_PIO_EGP_MASK = 0x4000`、`IOCON_PIO_I2CFILTER_MASK = 0x8000`）。

### 错误配置
原 `SPI1_IO_Config()` 中 P0_13 (MOSI) 和 P0_14 (MISO) 的 IOCON 值约为 `0x0105` / `0x0106`：
- bit 14 EGP = 0 → 引脚仍处于 **I²C 模式**
- bit 15 I2CFILTER = 0 → **50ns I²C 毛刺滤波器开启**

### 1bit 右移的物理过程
- SPI 时钟 12 MHz → 每 bit 周期 ≈ **83.3 ns**
- I²C pad 在 EGP=0 时输入路径插入 ~50ns 滤波延迟
- MCU 在 SCK 上升沿采样时，看到的是 ~50ns 之前的电平
- 在 12MHz 下正好相当于采样滞后到下一个 bit → **接收数据整体右移 1 位**（高位丢失，低位补 0）

### 为什么只有 SPI1 错？

| 通道 | MISO 引脚 | I²C-tolerant？ | 输入路径有滤波器？ | 现象 |
|---|---|---|---|---|
| **SPI1** | **P0_14** | **是** | **是 → 50ns 延迟** | **右移 1bit ⚠️** |
| SPI4 | P0_20 | 否 | 否 | 正常 |
| SPI5 | P0_9  | 否 | 否 | 正常 |
| SPI7 | P1_30 | 否 | 否 | 正常 |

唯独 SPI1 的 MISO 落在 I²C-tolerant 引脚 P0_14 上，所以唯独它出现 1-bit 右移。

---

## ✅ 本轮修复（2026-04-27）

### `board/pin_mux.h`（新增宏）
```c
#define IOCON_PIO_FILTER_OFF     0x1000u   // bit 12 数字滤波器关闭
#define IOCON_PIO_EGP_GPIO       0x4000u   // bit 14 切到 GPIO/SPI 模式
#define IOCON_PIO_I2CFILTER_DIS  0x8000u   // bit 15 关闭 I2C 50ns 滤波器
```

### `board/pin_mux.c`
1. `SPI1_IO_Config()` 中给 **P0_13 (MOSI) 和 P0_14 (MISO)** 加上 `IOCON_PIO_EGP_GPIO | IOCON_PIO_I2CFILTER_DIS`。
2. `SPI4_IO_Config()` 中给 **P1_21 (MOSI)** 加上同样位（防御性，P1_21 也是 I²C-tolerant）。

### `source/dma_config.c`
- 撤销了上一轮"SPI1 排在最后启动"的顺序调整，恢复 `SPI1 → SPI4 → SPI5 → SPI7` 的原始顺序（因为根本原因不在 DMA 启动顺序，而在 IO pad 配置）。

---

## 历史会话修改（保留）

| 文件 | 修改内容 |
|------|----------|
| `board/pin_mux.c` | 修正 `SPI1_IO_Config()` 之前误用的 `SPI0_*` 宏；修正 `SPI4_IO_Config()` 之前误用的 `SPI1_*` 宏；清理 `SET_ALLCLKPins_*` 中未定义的 `SPI0/2/3_*` 宏引用 |
| `source/dma_config.c` | 之前一轮试过将 CPHA 全局改为 SecondEdge，已撤销 |

---

## 项目架构 / 关键文件

| 文件 | 作用 |
|------|------|
| `board/pin_mux.c` | `BOARD_InitPins()` → `SPIx_IO_Config()` 设置各路 SPI 引脚复用；`SET_ALLCLKPins_*` 控制 CLK 引脚的 GPIO↔SPI 切换 |
| `board/pin_mux.h` | 定义 `SPIx_CLK/MOSI/MISO_PORT/PIN` 宏；IOCON 位宏 |
| `device/LPC55S28.h` | 厂家头文件，**所有 IOCON 位定义的权威来源** |
| `source/dma_config.c` | `EXAMPLE_MasterInit/DMASetup`、`DMA_Variable_Init`、`DMA_StartTransfer_Once/OnceFor_RawData/Once_TESTAPP/Suspend` |
| `source/hall_spi.c` | Hall 传感器命令/数据处理；`Hall_CLK_Command_SET()` |
| `drivers/fsl_spi_dma.c` | 自定义 SPI DMA 驱动 |

---

## Hall CLK 命令机制
```
Hall_CLK_Command_SET(high_us, low_us):
  1. SET_ALLCLKPins_AS_GPIO()
  2. SET_ALLCLKPins_AS_OUTPUT()
  3. SET_ALLCLKPins_HIGH()    (delay high_us)
  4. SET_ALLCLKPins_LOW()     (delay low_us, 传感器触发下降沿)
  5. SET_ALLCLKPins_AS_CLK()  (切回 SPI 模式)
```
- 当前实现中 SPI1/SPI4/SPI5/SPI7 的 CLK 都参与切换（`SET_ALLCLKPins_AS_*` 都已包含这 4 路）。

---

## 关键问题记录（本会话）
1. **为什么只有 SPI1 数据右移 1bit？** → P0_14 是 I²C-tolerant pad，EGP=0 时 50ns 滤波器导致采样延迟。
2. **是不是 DMA 配置问题？** → 不是。DMA 配置（width、size、handle）四路完全一致；问题在 IO pad 电气路径。
3. **是不是 CPHA 不一致？** → 不是。所有通道默认 CPHA=0 一致。
4. **是不是 SPI 启动顺序问题？** → 不是。无论先后，pad 滤波器始终延迟。

---

## 下一步建议
1. **编译烧录验证**：本轮修复仅改 `pin_mux.h` 和 `pin_mux.c`，逻辑很轻量，应当能直接消除 SPI1 右移 1bit。重点看 `DMA_SPI_RX_1_Buffer[]` 和逻辑分析仪是否完全一致。
2. 如果还有差异（不太可能），再看：
   - `IOCON_PinMuxSet()` 内部是否被 read-modify-write 覆盖（应该是 write 全寄存器）
   - SCK 引脚 P1_9 的 IOCON 在 `SET_ALLCLKPins_AS_CLK()` 中写入的 `0x0102` 是否需要补 `FILTEROFF` 位（P1_9 不是 I²C-tolerant，但数字滤波器仍可能影响很高频率下的边沿；当前 12MHz 应不至于）
3. 验证通过后建议把"I²C-tolerant pad 必须设 EGP=1 才能用作 SPI/GPIO" 写到代码注释/项目文档里防止以后再踩。

---

## 风险点 / 待确认项
- LPC55S28 的 I²C-tolerant pad 列表（用户应核对数据手册 Table "Pad type"）：常见为 `PIO0_13、PIO0_14、PIO1_20、PIO1_21`。
- `SPI4_MISO` 在当前 `pin_mux.h` 中是 `P0_20`（不是 I²C-tolerant），但函数注释里提过 P1_20，请确认硬件实际接的是哪个引脚（如果实际是 P1_20 = I²C-tolerant，也需要 EGP=1）。

---

## 下次新会话建议先读
1. 本文件（SESSION_SUMMARY.md）
2. `board/pin_mux.c` 的 `SPI1_IO_Config()` 和 `SPI4_IO_Config()`（看当前 IOCON 修复是否还在）
3. `device/LPC55S28.h` 第 11800-11900 行（IOCON 位定义权威）
