# Repository Guidelines

## Project Structure & Module Organization
Each top-level directory is a standalone CMake target. Executables live in `algorithm/`, `leetcode/`, and `playground/`, while `cache/`, `design_princples/`, `message_queue/`, and `some_knowledge/` build as static libraries plus small drivers (for example `cache/test/test_cache.cpp`). The root `CMakeLists.txt` wires every module; `memory_pool/` is temporarily excluded until the `nlohmann_json` dependency returns. Generated binaries land under `out/build/<preset>/` when using presets or `build/` when configuring manually.

## Build, Test, and Development Commands
Use the Ninja preset for a quick loop:
- `cmake --preset destiny` configures Debug output into `out/build/destiny/`.
- `cmake --build out/build/destiny` compiles every module.
- `./out/build/destiny/leetcode/twoSum2` (replace with your target) runs a single executable.
If you prefer a local build directory, `cmake -S . -B build && cmake --build build` mirrors the same layout. Regenerate after adding new `.cpp` files—CMake auto-registers them.

## Coding Style & Naming Conventions
Code is formatted with `.clang-format` (Google base, 4-space indents, brace-on-same-line). Run `clang-format -i <files>` before committing. Follow existing patterns: PascalCase for classes (`SolutionWithNthElement`), camelCase for functions and locals, SHOUT_CASE for compile-time constants. Keep headers in `include` blocks sorted and prefer `#pragma once` for new headers.

## Testing Guidelines
Module tests are lightweight executables. Place sanity checks beside the code (e.g., `cache/test/`), register them in the module `CMakeLists.txt`, and invoke them from the build tree: `./out/build/destiny/cache/test_cache`. Use `assert` or bespoke checks; if you introduce a framework, document the dependency and guard it behind CMake options. Aim to exercise both success paths and edge conditions relevant to the data structures you touch.

## Commit & Pull Request Guidelines
Recent history uses Conventional Commits (`feat:`, `fix:`, `refactor:`) plus concise summaries; include module context (`feat(message_queue): ...`) to aid navigation. For pull requests, describe the affected directories, note any new executables or presets, and attach sample command output that proves the run/test path. Link issues when available and flag breaking changes or external dependencies explicitly.
