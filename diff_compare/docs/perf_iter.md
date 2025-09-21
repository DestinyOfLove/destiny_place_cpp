# 性能迭代记录

| Commit | 场景 | 构建 | real_time_mean | bytes_per_second | PeakRSS_MB | 备注 |
| --- | --- | --- | --- | --- | --- | --- |
| d8f1f74 | Diff/IntHeavy/1000000Rows | Release | 180.44 ms | 78.99 MiB/s | 873.48 | 串行比较器，整列一次性载入；`--benchmark_repetitions=5` 基线（2025-09-21 重测） |
| eb23808 | Diff/IntHeavy/1000000Rows | Release | 172.86 ms | 82.40 MiB/s | 623.83 | 串行比较器 + 流式游标（逐行读取）；峰值 RSS 下降（重测） |
| 7c0ac3a | Diff/IntHeavy/1000000Rows | Release | 182.82 ms | 77.87 MiB/s | 511.97 | 串行比较器 + 流式游标 + 缓冲复用（初版内存池）；吞吐略降（重测） |
| 1b7f79e | Diff/IntHeavy/1000000Rows | Release | 180.03 ms | 79.10 MiB/s | 554.40 | 串行比较器 + 内存池 + `string_view` 裁剪；本地负载仍有波动 |
| 83c5981 | Diff/IntHeavy/1000000Rows | Release | 176.18 ms | 80.83 MiB/s | 523.68 | 串行比较器 + 内存池（1K 行批量）+ `string_view`；吞吐回升 |
| ee989f5 | Diff/IntHeavy/1000000Rows | Release | 169.28 ms | 84.07 MiB/s | 602.31 | 并行比较器尚未启用，串行下结合内存池带来额外收益 |
| e025cf9 | Diff/IntHeavy/1000000Rows | Release | 136.94 ms | 104.23 MiB/s | 1106.75 | **默认并行比较器**（自动按硬件线程拆分）+ 内存池；高线程数带来更高 RSS |
| 6a05782 | Diff/IntHeavy/1000000Rows | Release | 136.71 ms | 103.87 MiB/s | 914.40 | **默认并行比较器** + Boost.Pool 行缓存；RSS 较上次回落 |
| 49542b1 | Diff/IntHeavy/1000000Rows | Release | 142.49 ms | 99.95 MiB/s | 1065.85 | **默认并行比较器** + Boost.Pool + Boost.StringView；高系统负载下重测 |
| e46b973 | Diff/IntHeavy/1000000Rows | Release | 49.70 ms | 285.72 MiB/s | 2.90 | **默认并行比较器** + mmap 零拷贝行视图；吞吐飙升，RSS 大幅降低 |
| 2839b5f | Diff/IntHeavy/1000000Rows | Release | 55.89 ms | 254.63 MiB/s | 3.96 | **并行比较器** + 整数列缓存；吞吐略降，避免重复解析 |
