# 性能基准使用说明

diff_compare 项目提供了 `diff_compare_perf` 可执行文件用于评估在大规模数据集上的性能表现。本说明文档覆盖依赖安装、构建、运行及输出解读。

## 依赖准备

1. **安装 Google Benchmark**（通过 vcpkg）：
   ```bash
   vcpkg install benchmark
   ```
2. 在 CMake 配置命令中确保指定 vcpkg toolchain，例如：
   ```bash
   cmake -S . -B build \
     -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
     -DDIFF_COMPARE_ENABLE_PERF=ON
   ```
   若未安装 benchmark，CMake 会提示并跳过性能目标。

## 构建

在工程根目录执行：
```bash
cmake --build build --target diff_compare_perf
```
推荐使用 Release 构建以避免 Debug 模式干扰计时：
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release [...其他参数]
cmake --build build --config Release --target diff_compare_perf
```

## 运行基准

执行 `diff_compare_perf` 即会跑预设场景：
```bash
./build/diff_compare_perf
```
常用参数：
- `--benchmark_filter=Diff/IntHeavy`：仅运行匹配的场景。
- `--benchmark_min_time=1s`：设定最短测量时间。
- `--benchmark_out=result.json --benchmark_out_format=json`：将结果输出成 JSON 方便后续分析。

示例：
```bash
./build/diff_compare_perf --benchmark_filter=Diff/IntHeavy --benchmark_min_time=1s
```

## 输出解读

每个场景的输出包含：
- `Time`/`CPU`：真实时间与 CPU 时间（ns）。
- `items_per_second`：每秒处理的行数。
- `bytes_per_second`：吞吐量（以 MiB/s 表示）。
- `PeakRSS_MB`：运行迭代过程中记录的峰值常驻内存，便于观察内存占用趋势。
- `MismatchStride`：场景配置的差异频率；`Rows` 表示单次处理的行数。

### 常见警告
- **库以 Debug 编译**：若看到 `Library was built as DEBUG`，请改用 Release 构建或为 `diff_compare_perf` 添加 `NDEBUG` 定义。
- **Affinity/CPU 频率提示**：在 macOS 上 Google Benchmark 会提示无法设置线程绑核或无法获取 CPU 频率，属已知限制，不影响计时。可通过为 `diff_compare_perf` 添加 `BENCHMARK_FORCE_DISABLE_CPU_AFFINITY` 编译宏来消除绑核提示。

## 自定义场景

`perf/PerfBenchmarks.cpp` 中 `RegisterBenchmarks()` 定义了默认场景。新增场景时：
1. 根据需求补充 `PerfScenario`（指定类型、行数、字符串长度、差异步长）。
2. 调整数据生成逻辑（如需要新的数据模式）。
3. 重新构建并运行性能目标。

## 与优化配合

在进行懒加载、流式比较等性能优化前，建议先运行基准记录 baseline。优化后再次运行并对比 `items_per_second`、`PeakRSS_MB` 等指标，即可量化改动收益。
