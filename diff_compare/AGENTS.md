# Repository Guidelines

## Project Structure & Module Organization
- See `ARCHITECTURE.md` for a high-level view of the Series core and Column I/O layers.
- Headers live under `include/diff_compare/{core,io,app}/` with matching implementations in `src/{core,io,app}/`; keep layer boundaries clean when adding files.
- `test/` houses GoogleTest suites such as `test_diff_compare.cpp`; mirror the directory of the code under test.
- Generated build trees default to `build/`; Ninja, CMake presets, and ccache outputs stay outside source folders.

## Build, Test, and Development Commands
- Configure: `cmake --preset diff_compare` (adds the vcpkg toolchain, activates testing options).
- Rebuild quickly: `CCACHE_DISABLE=1 cmake --build build` when sandboxed ccache writes are blocked.
- Run unit tests: `ctest --preset diff_compare` or execute `./build/diff_compare_tests` after a build.
- Execute the CLI: `./build/diff_compare` reads column inputs and writes formatted diffs.

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
