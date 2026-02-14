# CardMatch - 纸牌消除游戏

## 项目简介
本项目是一个基于 **Cocos2d-x 3.17** 引擎开发的纸牌消除类休闲游戏。玩家需要通过点击主牌区的纸牌，与底牌堆顶部的牌进行数字接龙（±1）消除，支持备用牌抽取和无限回退功能。

项目严格遵循 **MVC (Model-View-Controller)** 架构设计，实现了高内聚低耦合的代码结构。

## 开发环境
*   **引擎版本**: Cocos2d-x 3.17.x
*   **开发语言**: C++ 11+
*   **构建工具**: CMake 3.10+
*   **推荐 IDE**: Visual Studio 2019 / VS Code (Windows)
*   **设计分辨率**: 1080 x 2080 (竖屏)

## 目录结构

## 编译与运行 (Windows)

1.  确保已安装 CMake 和 Visual Studio C++ 编译环境。
2.  进入项目 `build` 目录：
    ```powershell
    cd build
    ```
3.  生成工程文件 (指定通过 Visual Studio 2019 x86 构建)：
    ```powershell
    cmake .. -G "Visual Studio 16 2019" -A Win32
    ```
4.  编译项目：
    ```powershell
    cmake --build . --config Debug
    ```
5.  运行生成的 `bin/Debug/CardMatch.exe`。

## 功能特性
1.  **自动布局**: 支持通过 JSON 配置关卡，或在配置为空时自动生成自适应布局。
2.  **子控制器解耦**: 将游戏逻辑拆分为 `GameController`(总控)、`PlayFieldController`(桌面) 和 `StackController`(牌堆)。
3.  **平滑动画**: 实现了发牌、收牌、回退的平滑位移动画。
4.  **回退系统**: 记录每一步操作状态，支持撤销发牌和撤销消除。
5.  **资源管理**: 统一的资源路径搜索配置。

---