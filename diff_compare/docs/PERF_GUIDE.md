# 性能基准使用说明

`diff_compare` 提供 `diff_compare_perf` 用于衡量大规模数据下的运行效率。本指南覆盖依赖安装、构建、执行以及结果解读，方便比较不同提交的表现。

## 依赖准备
1. 通过 vcpkg 安装 Google Benchmark：
   ```bash
   vcpkg install benchmark
   ```
2. 配置时指定 vcpkg 工具链，例如：
   ```bash
   cmake -S . -B build \
     -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
     -DDIFF_COMPARE_ENABLE_PERF=ON
   ```
   若未装 benchmark，CMake 会提示并跳过性能目标。

## 构建流程
- 若不使用预设，可直接执行：
  ```bash
  cmake --build build --target diff_compare_perf
  ```
- 推荐使用仓库自带的 Release 预设：
  ```bash
  cmake --preset diff_compare_perf_release
  cmake --build --preset diff_compare_perf_release
  ```

## 运行基准
- 默认执行：
  ```bash
  ./build/perf_release/diff_compare_perf
  ```
- 常用参数：
  - `--benchmark_filter=Diff/IntHeavy` 仅运行指定场景；
  - `--benchmark_min_time=1s` 控制最短测试时长；
  - `--benchmark_out=perf.json --benchmark_out_format=json` 将结果保存为 JSON 以便对比。

示例：
```bash
./build/perf_release/diff_compare_perf \
  --benchmark_filter=Diff/IntHeavy \
  --benchmark_repetitions=5 \
  --benchmark_out=perf.json \
  --benchmark_out_format=json
```

## 输出解读
- `Time/CPU`：真实时间与 CPU 时间（纳秒）。
- `items_per_second`：每秒处理行数。
- `bytes_per_second`：换算后的吞吐量（MiB/s）。
- `PeakRSS_MB`：运行期间观测到的峰值常驻内存，mmap+整型缓存场景通常为数 MiB。
- `MismatchStride`、`Rows`：场景配置的差异步长与行数。

常见告警：
- **Debug 编译提示**：若看到 `Library was built as DEBUG`，请改用 Release 或定义 `NDEBUG`。
- **CPU 频率/绑核警告**：macOS 上无法设置线程亲和性属正常，可在编译时加 `BENCHMARK_FORCE_DISABLE_CPU_AFFINITY` 消除提示。

## 自定义场景
- 在 `perf/PerfBenchmarks.cpp` 的 `RegisterBenchmarks()` 中新增场景，指定列类型、行数、字符串长度、差异频率等参数。
- 调整数据生成逻辑后重新构建并运行性能目标即可。

## 与优化联动
- 在进行 I/O、算法优化前，先记录 baseline。
- 完成改动后再次运行基准，对比 `items_per_second`、`PeakRSS_MB` 等指标量化收益。
- 每次测试结束都应将结果与 commit id 一并追加到 `docs/perf_iter.md`，保持性能历史可追溯。
