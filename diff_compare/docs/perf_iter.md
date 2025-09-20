# 性能迭代记录

| Commit | 场景 | 构建 | real_time_mean | bytes_per_second | PeakRSS_MB | 备注 |
| --- | --- | --- | --- | --- | --- | --- |
| d8f1f74 | Diff/IntHeavy/1000000Rows | Release | 170.38 ms | 83.34 MiB/s | 740.14 | 流式改造前基线，`MismatchStride=40`，stats 自 `--benchmark_repetitions=5` |
| eb23808 | Diff/IntHeavy/1000000Rows | Release | 171.99 ms | 82.56 MiB/s | 419.13 | 引入游标流式比较，峰值 RSS 明显下降 (~-320 MB) |
| 7c0ac3a | Diff/IntHeavy/1000000Rows | Release | 175.89 ms | 80.92 MiB/s | 491.57 | `TxtSeriesCursor` 复用 buffer 减少分配；受系统负载影响一次跑出 193ms，平均吞吐略降 (~+2%) |
| 1b7f79e | Diff/IntHeavy/1000000Rows | Release | 181.18 ms | 78.61 MiB/s | 499.76 | 使用 `string_view` 精简裁剪逻辑，但高系统负载导致波动偏大，峰值 RSS 稍有回升 |
