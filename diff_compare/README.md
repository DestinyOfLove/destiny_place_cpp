# diff_compare

`diff_compare` 是一个面向列文本（逐行数据）的对比工具，当前聚焦于 macOS 环境，通过 `mmap` 实现零拷贝读取，并在整型列上增量解码，追求高吞吐与低内存占用。本文档简要介绍工程架构与扩展点，方便新成员上手。

## 架构设计概览

项目采用“核心/边界/应用”三层分离模型，详细结构见 `ARCHITECTURE.md`（包含 PlantUML 图）：

- **核心层**：`SeriesDescriptor`、`SeriesData`、`SeriesComparator` 等纯业务对象。`SeriesData` 支持字符串视图与整型缓存；比较器根据 `ValueType` 选择并行策略，数据量较小时自动降为单线程。
- **I/O 层**：`TxtColumnInputProvider` 负责通过 `mmap` 读取列文件并构建 `SeriesData`；`ColumnParser` 清洗首行并推断类型；`TxtDiffOutputWriter` 与 `PlainTextOutputFormatter` 负责输出文本结果。
- **应用层**：`ColumnProcessingPipeline` 串联输入、比较、输出；`ColumnDiffApp` 是 CLI 入口。

这种划分使得 `SeriesData` 与比较器不依赖具体 I/O，便于以后接入数据库、网络流等其他数据源，同时 `SeriesData` 内部的懒加载策略也减少了内存分配。

## 性能关键点

- **零拷贝读取**：使用 `mmap` 将列文件映射到用户态内存，配合 `boost::string_view` 直接引用，避免中间缓冲。
- **整型列增量解码**：在读取阶段同时尝试把整型列解析成 `int64_t` 数组，比较器优先使用该缓存，必要时退回字符串解析。
- **并行比较器**：根据 `hardware_concurrency` 动态拆分任务，对大数据利用多核，数据量较小时自动降为单线程执行。
- **性能追踪**：`docs/perf_iter.md` 按提交记录真实耗时、吞吐与峰值 RSS，配合 `docs/PERF_GUIDE.md` 中的运行说明，确保每次优化有可对比的量化数据。

## 可扩展性

### 新的数据类型
1. 在 `valueTypeFromHeader` 注册新的前缀，并扩展 `ValueType` 枚举。
2. 实现对应的 `SeriesComparator`（默认并行实现，内部根据任务规模自适应线程数）。
3. 在 `SeriesComparatorFactory::create` 的 `switch` 中返回该比较器。
4. 补充解析与比较的单元测试。

### 新的输入/输出渠道
- **输入**：实现新的 `SeriesInputProvider`（例如 CSV、数据库驱动），只要返回 `SeriesData` 即可复用核心层。
- **输出**：实现 `SeriesOutputWriter`/`OutputFormatter`，可将 `SeriesDiff` 序列化为 JSON、二进制等格式。
- **多前端**：若需要 GUI 或服务化，可以新建入口模块，复用现有管线与比较逻辑。

## 快速开始
1. 准备依赖：`vcpkg install fmt boost-utility benchmark`（基于项目已有的 vcpkg toolchain）。
2. 配置与构建：
   ```bash
   cmake --preset diff_compare
   cmake --build build
   ```
3. 运行单元测试：`ctest --preset diff_compare`
4. 执行 CLI：`./build/diff_compare A.txt B.txt diff_output.txt`
5. 跑性能基准：`cmake --build --preset diff_compare_perf_release`，再执行 `./build/perf_release/diff_compare_perf --benchmark_filter=Diff/IntHeavy`

更详细的协作规范、架构说明与性能流程，请分别参见：
- `AGENTS.md` – 协作与代码规范
- `ARCHITECTURE.md` – 分层设计与关键技术
- `docs/PERF_GUIDE.md` – 性能测试指南
- `docs/perf_iter.md` – 性能历史记录

欢迎贡献改进，建议先在性能基线基础上验证，再提交 PR 说明影响与测试结果。
