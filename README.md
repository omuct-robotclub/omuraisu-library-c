# omuraisu-library

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C17](https://img.shields.io/badge/C-17-blue.svg)](https://en.cppreference.com/w/c/language)

ロボット開発において頻繁に使用される基本的な機能を提供する C ライブラリ群です。

PlatformIO / CMake の両方に対応しており、組み込み（CubeMX）からデスクトップまで幅広い環境で利用できます。

## 目次

- [インストール](#インストール)
- [モジュール一覧](#モジュール一覧)
  - [coordinate — 2D 座標演算](#coordinate--2d-座標演算)
  - [chassis — メカナムホイール制御](#chassis--メカナムホイール制御)
  - [pid — PID 制御](#pid--pid-制御)
  - [dji — DJI ロボマスモーター制御](#dji--dji-ロボマスモーター制御)
  - [can — CAN バス抽象化](#can--can-バス抽象化)
  - [controller — コントローラ入力](#controller--コントローラ入力)
  - [servo — サーボ制御](#servo--サーボ制御)
- [サンプルコード](#サンプルコード)
- [ビルド方法](#ビルド方法)
- [テスト](#テスト)
- [ライセンス](#ライセンス)

---

## インストール

### PlatformIO

`platformio.ini` に以下を追加してください。

```ini
lib_deps = https://github.com/omuct-robotclub/omuraisu-library.git
```

### CubeMX

**Project Manageer**で **Toolchain / IDE**を**CMake**に設定し、CMakeのインストール方法に従ってください。また、追加で以下の設定が必要です。
`set(CMAKE_C_STANDARD 11)`を`set(CMAKE_C_STANDARD 17)`に変更します。

```cmake
target_compile_definitions(omuraisu_can PRIVATE
    USE_HAL_DRIVER
    <使用MCU名>
)
target_include_directories(omuraisu_can PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/<使用MCU名>_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/<使用MCU名>_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/<使用MCU名>/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)
```

<使用MCU名>にはSTM32F446xx などが入ります。環境に合うように変更してください。

### CMake（FetchContent）

```cmake
include(FetchContent)
FetchContent_Declare(
  omuraisu
  GIT_REPOSITORY https://github.com/omuct-robotclub/omuraisu-library.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(omuraisu)

target_link_libraries(your_target PRIVATE omuraisu)
```

### CMake（システムインストール）

```bash
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

```cmake
find_package(omuraisu REQUIRED)
target_link_libraries(your_target PRIVATE omuraisu::omuraisu)
```

> 個別モジュールのみをリンクすることも可能です（`omuraisu::coordinate`, `omuraisu::omuraisu_pid` など）。

---

## モジュール一覧

### coordinate — 2D 座標演算

**ヘッダ:** `coordinate/coordinate.h`

2D の直交座標・極座標および速度を扱う C 構造体と演算関数を提供します。

| 型                | 説明                                      |
| ----------------- | ----------------------------------------- |
| `Coordinate`      | 直交座標（`x`, `y`, `ang`, `axis_ang`）   |
| `CoordinatePolar` | 極座標（`r`, `theta`, `ang`, `axis_ang`） |
| `Velocity`        | `Coordinate` のエイリアス                 |
| `VelocityPolar`   | `CoordinatePolar` のエイリアス            |

```c
#include "coordinate/coordinate.h"

Coordinate a = om_coordinate_init_value(1.0f, 2.0f, 0.0f, 0.0f);
Coordinate b = om_coordinate_init_value(3.0f, 4.0f, 0.0f, 0.0f);
om_coordinate_add(&a, &b);

CoordinatePolar p = om_coordinate_polar_from_rectangular(&a);
float d = om_coordinate_distance(&a, &b);
```

### chassis — メカナムホイール制御

**ヘッダ:** `chassis/mecanum.h`

現行の C 版では **未実装** です。現在の `mecanum.h` は C++ 形式の旧実装で、C API としては公開されていません。

### pid — PID 制御

**ヘッダ:** `pid/pid.h`

出力リミット付きの PID コントローラを C API で提供します。

$$
u(t) = K_p \, e(t) + K_i \int_0^t e(\tau)\,d\tau + K_d \frac{de}{dt}
$$

| 型              | 説明                                             |
| --------------- | ------------------------------------------------ |
| `PidGain`       | ゲイン（`kp`, `ki`, `kd`）                       |
| `PidParameter`  | ゲイン + 出力上下限（`gain`, `min`, `max`）      |
| `PidController` | PID 制御の内部状態（前回偏差・積分値などを保持） |

```c
#include "pid/pid.h"

PidParameter param = {
  .gain = {.kp = 1.0f, .ki = 0.1f, .kd = 0.01f},
  .min = -100.0f,
  .max = 100.0f,
};
PidController pid = om_pid_init(param);

float output = om_pid_calc(&pid, 100.0f, 0.0f, 0.01f);
om_pid_reset(&pid);
```

### dji — DJI ロボマスモーター制御

**ヘッダ:** `dji/robomas.h`, `dji/robomas_core.h`

DJI M3508 / C620 モーター向けの CAN 制御ロジックを C API で提供します。最大 8 軸を同時制御できます。

| 型            | 説明                                               |
| ------------- | -------------------------------------------------- |
| `RobomasData` | モーターフィードバック（角度・回転数・電流・温度） |
| `RobomasCore` | プラットフォーム非依存のコアロジック               |
| `Robomas`     | `CanBus` と `RobomasCore` をまとめた制御ハンドラ   |

```c
#include "dji/robomas.h"

Robomas rm = om_rm_init(&bus);
om_rm_set_output(&rm, 5000, 1);
om_rm_set_output_percent(&rm, 0.5f, 2);
om_rm_write(&rm);

int motor_idx = om_rm_read(&rm);
if (motor_idx >= 0) {
  uint16_t angle = om_rm_get_angle(&rm, motor_idx + 1);
  int16_t rpm = om_rm_get_rpm(&rm, motor_idx + 1);
}
```

### can — CAN バス抽象化

**ヘッダ:** `can/can_interface.h`, `can/can_cube.h`, `can/can_stm32.h`, `can/can_mbed.h`

プラットフォーム非依存の CAN バスインターフェースを C API で定義します。

| 型                | 説明                                       |
| ----------------- | ------------------------------------------ |
| `CanMessage`      | CAN メッセージ（ID, データ, 長さ）         |
| `CanBus`          | 抽象 CAN バスインターフェース              |
| `CanCube`         | Cube HAL 向けの受信キュー付き CAN ブリッジ |
| `CanStm32Context` | STM32 HAL 用の登録コンテキスト             |

```c
#include "can/can_interface.h"

CanMessage msg = {0};
msg.id = 0x200;
msg.data[0] = 0xFF;
msg.len = 1;

can_bus_write(&bus, &msg);
```

Cube HAL で受信コールバックと組み合わせる場合は、`can_cube` に `can_stm32` の ops を渡して使います。

```c
CanCube cube;
CanStm32Context stm32;
CanCubeOps ops;

can_stm32_context_init(&stm32, &cube, &hcan1, CAN_STM32_KIND_CAN, 0);
can_stm32_make_ops(&ops);
can_cube_init(&cube, &stm32, &ops);
can_stm32_register(&stm32);
can_cube_start_read(&cube);
CanBus bus = can_cube_bus(&can_cube);


void Can_Callback(const CanMessage* msg, void* user_arg);
can_cube_set_rx_callback(&can_cube, Can_Callback, NULL); // CAN受信時のコールバックを登録(任意)

// HAL callback 内
// HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
// {
//   can_stm32_dispatch_rx(hcan);
// }
```

### controller — コントローラ入力

**ヘッダ:** `controller/controller_core.h`, `controller/controller_can.h`, `controller/controller_serial.h`

現行の C 版では **未実装** です。ヘッダは存在しますが、C++ 形式の旧実装であり C API としては未提供です。

### servo — サーボ制御

**ヘッダ:** `servo/servo_core.h`

現行の C 版では **未実装** です。ヘッダは存在しますが、C++ クラスベースの旧実装のため C API は未提供です。

---

## サンプルコード

`examples/` ディレクトリにサンプルプログラムがあります。

| ファイル                 | 内容                                     |
| ------------------------ | ---------------------------------------- |
| `coordinate_example.cpp` | 座標の生成・変換・演算                   |
| `mecanum_example.cpp`    | メカナムホイールの速度計算               |
| `pid_example.cpp`        | PID 制御シミュレーション（100 ステップ） |

サンプルをビルドするには：

```bash
cmake -DBUILD_EXAMPLES=ON ..
cmake --build .
```

---

## ビルド方法

### 要件

- CMake 3.14 以上
- C++17 対応コンパイラ

### ビルド手順

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### ビルドオプション

| オプション       | 説明                       | デフォルト |
| ---------------- | -------------------------- | ---------- |
| `BUILD_EXAMPLES` | サンプルプログラムをビルド | `OFF`      |
| `BUILD_TESTS`    | テストをビルド             | `OFF`      |

---

## テスト

Google Test を使用しています。

```bash
mkdir build && cd build
cmake -DBUILD_TESTS=ON ..
cmake --build .
ctest --output-on-failure
```

| テストファイル              | 内容                         |
| --------------------------- | ---------------------------- |
| `tests/coordinate_test.cpp` | 座標の構築・演算・変換       |
| `tests/pid_test.cpp`        | 比例制御・出力制限・リセット |

---

## ライセンス

[MIT License](LICENSE)

## 作者

[bit](https://github.com/NekoChan9382)
