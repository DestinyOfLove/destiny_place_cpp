# 架构概览

```plantuml
#!docs/architecture.puml
```

> 生成图片：`plantuml docs/architecture.puml`

## 分层说明
- **核心层（Series Core）**：包含 `SeriesDescriptor`、`SeriesData`、`SeriesDiff` 以及比较策略。`SeriesData` 既维护懒加载的字符串视图，也缓存整型列的 `int64_t` 数组，比较器可以直接消费这些数据而无需重复解析。所有比较策略统一实现 `SeriesComparator` 并返回 `SeriesDiff`。
- **I/O 边界层（Column I/O Boundary）**：`SeriesInputProvider`/`SeriesOutputWriter` 抽象与核心解耦，默认实现包括 `ColumnParser`、`TxtColumnInputProvider`、`TxtDiffOutputWriter`，负责适配换行分隔的列文件。
- **应用层（Application Shell）**：`ColumnProcessingPipeline` 负责调度输入、比较与输出，`ColumnDiffApp` 解析 CLI 参数并组装依赖。

## 数据流程
1. CLI 接收列文件路径（A、B、输出），委托 `ColumnProcessingPipeline`。
2. `TxtColumnInputProvider` 通过 `mmap` 零拷贝读取列文件，借助 `ColumnParser` 校正表头并推断 `ValueType`，若为整型列则同步生成 `int64_t` 缓冲，最终封装为 `SeriesData`。
3. `SeriesComparatorFactory` 根据 `SeriesDescriptor::type()` 返回对应比较器。
4. 比较器得到 `SeriesDiff`，由 `PlainTextOutputFormatter` + `TxtDiffOutputWriter` 写回磁盘。

## 扩展指引
### 新增 ValueType
1. 在 `valueTypeFromHeader` 注册新前缀，并在 `toString` 中补充名称。
2. 实现对应比较策略（如 `DateSeriesComparator`）。
3. 在 `SeriesComparatorFactory` 的 `switch` 中返回该比较器。
4. 编写解析、比较与集成测试，覆盖正常与异常路径。

### 支持新输入/输出
- 实现新的 `SeriesInputProvider`（可连接 CSV/数据库等数据源），返回 `SeriesData` 即可复用核心逻辑。
- 实现新的 `SeriesOutputWriter`（例如 JSON），共享 `SeriesDiff` 与格式化器。
- 若需要多前端，可在自定义 `main` 中重新装配依赖。

## 目录与命名
- 核心类以 `Series*` 命名，类型枚举使用 `ValueType`，I/O 适配器使用 `Column*` 前缀。
- 头文件位于 `include/diff_compare/{core,io,app}/`，实现位于 `src/` 同级目录；应用相关入口放在 `src/app/`，测试放在 `test/`。

## 测试策略
- GoogleTest 用例集中在 `test/test_diff_compare.cpp`，新增组件需编写对应测试（`TEST(Suite, Case)`）。
- 在提交前运行 `cmake --preset diff_compare && cmake --build build && ctest --preset diff_compare` 验证。

## 关键技术与性能要点
- **mmap 零拷贝**：避免大文件重复读写，比较线程按需访问内存页，I/O 与计算自然重叠。
- **整型列缓存**：在输入阶段解析为 `int64_t`，比较器直接做数值运算，遇到非法字符自动退回字符串模式。
- **并行比较**：数值/文本比较器按 `hardware_concurrency` 分块，任务较小时自动退化为单线程执行，无需单独串行实现。
- **懒加载视图**：`SeriesData` 仅在需要时才生成字符串或视图，节省内存。
- **性能追踪**：`docs/perf_iter.md` 记录每次优化后的耗时与 RSS，`docs/PERF_GUIDE.md` 描述基准运行方式，确保性能改动可量化评估。
