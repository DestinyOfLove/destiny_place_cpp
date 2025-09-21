# Architecture Overview

```plantuml
#!docs/architecture.puml
```

> 生成图：`plantuml docs/architecture.puml`

## Layered Design
- **Series Core**: Domain模型包含 `SeriesDescriptor`、`SeriesData`、`SeriesDiff` 以及比较策略。`SeriesData` 既可维护懒加载的字符串视图，也会在整型列上缓存 `int64_t` 数组，比较器因此可以跳过重复的文本解析。比较策略统一实现 `SeriesComparator` 接口并生成 `SeriesDiff`。
- **Column I/O Boundary**: Generic `SeriesInputProvider`/`SeriesOutputWriter` contracts decouple the core from transport concerns; the default text stack (`ColumnParser`, `TxtColumnInputProvider`, `TxtDiffOutputWriter`) adapts them to newline-delimited columns.
- **Application Shell**: `ColumnProcessingPipeline` orchestrates providers, comparator factories, and writers. `ColumnDiffApp` parses CLI arguments and wires dependencies.

## Data Flow
1. CLI receives three paths (A, B, output) and delegates to `ColumnProcessingPipeline`.
2. A `SeriesInputProvider` reads each source；默认 `TxtColumnInputProvider` 通过 `mmap` 零拷贝到内存，使用 `ColumnParser` 规整首行并推断 `ValueType`，对于整数列同时写入 `int64_t` 缓冲，最终封装为 `SeriesData`。
3. `SeriesComparatorFactory` selects a `SeriesComparator` based on `SeriesDescriptor::type()` and executes the comparison.
4. `SeriesDiff` returns to the boundary where a `SeriesOutputWriter` (e.g., `TxtDiffOutputWriter` + `PlainTextOutputFormatter`) persists results.

## Extension Checklist
### Adding a New Value Type
1. Register a prefix in `valueTypeFromHeader` and extend `toString` with the new enum value.
2. Implement a `SeriesComparator` specialized for the type (e.g., `DateSeriesComparator`).
3. Update `SeriesComparatorFactory::create` to build the comparator (consider refactoring to a registry when multiple custom comparators exist).
4. Add unit tests covering parsing, comparator behavior, and pipeline integration.

### Supporting Alternative Inputs/Outputs
- Implement a new `SeriesInputProvider` (wrapping a bespoke parser or data source such as CSV/DB) that still returns `SeriesData`.
- Provide a matching `SeriesOutputWriter` (e.g., JSON output) while reusing `SeriesDiff` and optionally sharing formatters.
- Register the new components in a factory or wire them in an alternate `main` if the build needs multiple front ends.

## Naming & Organization
- Use `Series*` for core domain classes, `ValueType` for the inferred type enum, and reserve `Column*` prefixes for I/O adapters.
- Group headers under `include/diff_compare/core|io|app/` and mirror the structure in `src/core|io|app/` so each component keeps its layer-local dependencies obvious.
- Keep executable wiring (CLI, main) in `src/app/` and high-level tests under `test/`, mirroring the modules they exercise.

## Testing Strategy
- Unit tests in `test/test_diff_compare.cpp` demonstrate end-to-end usage through the pipeline. New components should receive suite-specific tests (`TEST(NewComparator, Scenario)`).
- Run `cmake --preset diff_compare`, `cmake --build build`, and `ctest --preset diff_compare` to validate changes before committing.

## Key Techniques & Performance Considerations
- **mmap 零拷贝输入**：`TxtColumnInputProvider` 将文本列直接映射到内存，配合 `boost::string_view` 避免将整列拷贝进自有缓冲，I/O 与比较阶段自然重叠。
- **整型列的增量解码**：在 mmap 扫描阶段即尝试生成 `int64_t` 缓冲；比较器优先读取该缓冲，从而绕过重复的字符串解析。若遇到异常字符自动退化为字符串视图模式，保证兼容性。
- **并行比较器**：`ParallelNumericSeriesComparator`/`ParallelTextSeriesComparator` 依据 `std::thread::hardware_concurrency()` 拆分工作块，同时保持串行回退路径以适配小数据集。
- **懒加载与视图缓存**：`SeriesData` 催化出的字符串视图/整型缓冲均为懒生成，只有比较阶段真正访问时才触发；同时保留 cursor 工厂以支持未来的流式读取或网络输入。
- **性能验证工作流**：`docs/perf_iter.md` 记录各个提交下的吞吐与峰值 RSS，配合 `docs/PERF_GUIDE.md` 中的构建/运行说明，确保每次优化都有可对比的量化数据。
