# Repository Guidelines

## Project Structure & Module Organization
- See `ARCHITECTURE.md` for a high-level view of the Series core and Column I/O layers.
- Performance history lives in `docs/perf_iter.md`; append new benchmark结果（附 commit id）后再提交代码。
- Headers live under `include/diff_compare/{core,io,app}/` with matching implementations in `src/{core,io,app}/`; keep layer boundaries clean when adding files.
- `test/` houses GoogleTest suites such as `test_diff_compare.cpp`; mirror the directory of the code under test.
- Generated build trees default to `build/`; Ninja, CMake presets, and ccache outputs stay outside source folders.
- `TxtColumnInputProvider` 通过 `mmap` + `boost::string_view` 零拷贝读取文本列（Windows 下自动回退到流式解析），并在整型列上立即缓存 `int64_t` 以供比较器直接使用。

## Build, Test, and Development Commands
- Configure: `cmake --preset diff_compare` (adds the vcpkg toolchain, activates testing options).
- Rebuild quickly: `CCACHE_DISABLE=1 cmake --build build` when sandboxed ccache writes are blocked.
- Run unit tests: `ctest --preset diff_compare` or execute `./build/diff_compare_tests` after a build.
- Execute the CLI: `./build/diff_compare` reads column inputs and writes formatted diffs.
- 依赖：零拷贝解析依赖 Boost.Utility；缺少时执行 `vcpkg install boost-utility`。

## Coding Style & Naming Conventions
- The repository pins C++11 with Google-style formatting; run `clang-format -i <files>` before committing.
- Prefer PascalCase for classes (`SeriesDescriptor`), camelCase for functions/locals, and SHOUT_CASE for constants.
- Keep headers guarded with `#pragma once`, sort includes lexicographically, and avoid mixing tabs with four-space indents.

## Testing Guidelines
- GoogleTest drives unit coverage; co-locate fixtures beside the code they validate and use descriptive `TEST(Suite, Case)` names.
- Exercise both happy paths and error handling—e.g., mismatched column descriptors—mirroring production scenarios.
- Register new executables in `CMakeLists.txt` and ensure `DIFF_COMPARE_ENABLE_TESTS` gates optional test dependencies.

## Commit & Pull Request Guidelines
- Follow Conventional Commits with module scopes (`fix(diff_compare): ...`, `build(diff_compare): ...`).
- Describe functional intent, reference affected directories, and paste representative command output (build/test snippets).
- Link issues when available, flag breaking changes, and document external dependencies or toolchain prerequisites.

## Agent Workflow Tips
- Respect existing Git history: never amend commits unless requested and verify with `git status -sb` before staging.
- Use `rg` for code searches and keep edits ASCII unless the file already adopts Unicode artifacts.
