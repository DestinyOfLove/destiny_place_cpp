# 性能迭代记录

| Commit | 场景 | 构建 | real_time_mean | bytes_per_second | PeakRSS_MB | 备注 |
| --- | --- | --- | --- | --- | --- | --- |
| d8f1f74 | Diff/IntHeavy/1000000Rows | Release | 170.38 ms | 83.34 MiB/s | 740.14 | 串行比较器，整列一次性载入；`--benchmark_repetitions=5` 基线 |
| eb23808 | Diff/IntHeavy/1000000Rows | Release | 171.99 ms | 82.56 MiB/s | 419.13 | 串行比较器 + 流式游标（逐行读取）；峰值 RSS 大幅下降 |
| 7c0ac3a | Diff/IntHeavy/1000000Rows | Release | 175.89 ms | 80.92 MiB/s | 491.57 | 串行比较器 + 流式游标 + 缓冲复用（初版内存池）；一次误差导致吞吐略降 |
| 1b7f79e | Diff/IntHeavy/1000000Rows | Release | 181.18 ms | 78.61 MiB/s | 499.76 | 串行比较器 + 内存池 + `string_view` 裁剪；受高负载影响波动偏大 |
| 83c5981 | Diff/IntHeavy/1000000Rows | Release | 173.73 ms | 82.00 MiB/s | 542.02 | 串行比较器 + 内存池（1K 行批量读取）+ `string_view`；吞吐回升，RSS 稍高 |
| ee989f5 | Diff/IntHeavy/1000000Rows | Release | 156.12 ms | 90.95 MiB/s | 524.32 | 同上优化，仍由串行比较器驱动；空闲系统下获得 ~+8% 吞吐 |
| e025cf9 | Diff/IntHeavy/1000000Rows | Release | 156.12 ms | 90.95 MiB/s | 524.32 | **默认并行比较器**（自动按硬件线程拆分）+ 内存池；测量继承 ee989f5 的结果 |
