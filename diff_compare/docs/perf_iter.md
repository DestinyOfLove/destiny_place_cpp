# 性能迭代记录

| Commit | 场景 | 构建 | real_time_mean | bytes_per_second | PeakRSS_MB | 备注 |
| --- | --- | --- | --- | --- | --- | --- |
| d8f1f74 | Diff/IntHeavy/1000000Rows | Release | 170.38 ms | 83.34 MiB/s | 740.14 | 流式改造前基线，`MismatchStride=40`，stats 自 `--benchmark_repetitions=5` |
| eb23808 | Diff/IntHeavy/1000000Rows | Release | 171.99 ms | 82.56 MiB/s | 419.13 | 引入游标流式比较，峰值 RSS 明显下降 (~-320 MB) |
