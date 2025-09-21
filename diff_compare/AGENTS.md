# 仓库协作指南

## 项目结构与模块
- 架构总览请阅读 `ARCHITECTURE.md`，文中附有 PlantUML 图说明核心组件与数据流。
- 性能基线记录在 `docs/perf_iter.md`，每次运行基准测试后请附上 `git rev-parse --short HEAD` 与关键指标再提交。
- 头文件放在 `include/diff_compare/{core,io,app}/`，实现位于 `src/{core,io,app}/`，保持层级依赖清晰。
- 测试代码位于 `test/`，使用 GoogleTest，目录结构与被测模块一致（示例：`test_diff_compare.cpp`）。
- 构建输出默认位于 `build/` 或 `out/build/<preset>/`，不要将中间文件写回源码目录。
- `TxtColumnInputProvider` 在 macOS 下通过 `mmap` + `boost::string_view` 零拷贝读取文本列，并在整型列上即时缓存 `int64_t` 数据供比较器直接使用。

## 构建与测试命令
- 配置：`cmake --preset diff_compare`（启用 vcpkg toolchain 与测试开关）。
- 编译：`cmake --build build`；若受限环境阻止 ccache，可临时 `CCACHE_DISABLE=1 cmake --build build`。
- 单元测试：`ctest --preset diff_compare` 或运行 `./build/diff_compare_tests`。
- CLI：`./build/diff_compare`，传入两列数据与输出路径即可生成差异结果。
- 依赖：零拷贝实现依赖 Boost.Utility，缺失时执行 `vcpkg install boost-utility`。

## 代码风格与命名
- 全局使用 C++11，并遵循 `.clang-format`（Google 风格）；提交前执行 `clang-format -i <files>`。
- 命名规则：类用 PascalCase（如 `SeriesDescriptor`），函数/局部变量用 camelCase，常量用 SHOUT_CASE。
- 新增头文件应使用 `#pragma once`，`#include` 按字典序排序，避免混用制表符与空格。

## 测试要求
- 使用 GoogleTest，`TEST(Suite, Case)` 名称需突出意图，测试文件与源码共存。
- 同时覆盖正常流程与异常情况（例如列头不匹配、行数不同）。
- 新增可执行文件记得在 `CMakeLists.txt` 注册，并确保通过 `DIFF_COMPARE_ENABLE_TESTS` 控制可选依赖。

## 提交流程
- 遵循 Conventional Commits，并加上模块范围（如 `fix(core): ...`、`perf(io): ...`）。
- 提交信息需说明改动目的、影响范围，并附上关键构建/测试命令输出。
- 合并前请说明是否引入新依赖或破坏兼容性，必要时关联相关 issue。

## 协作注意事项
- 尊重既有历史，除非明确要求禁止私自 `amend`/`rebase` 已共享的提交；提交前务必 `git status -sb`。
- 代码检索优先使用 `rg`（ripgrep），保持文件使用 ASCII 字符集，除非项目已有其他编码。
