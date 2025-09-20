# Architecture Overview

## Layered Design
- **Series Core**: Pure domain model (`SeriesDescriptor`, `SeriesData`, `SeriesDiff`) plus comparison strategies. This layer is unaware of files or "columns" and only reasons about homogeneous series of string values. Comparators implement `SeriesComparator` and return `SeriesDiff` objects.
- **Column I/O Boundary**: File-oriented adapters (`ColumnParser`, `ColumnInputProvider`, `TxtDiffOutputWriter`) translate between text files and the core series model. They enforce header prefixes, build `SeriesDescriptor`s, and hand off work to the core.
- **Application Shell**: `ColumnProcessingPipeline` orchestrates providers, comparator factories, and writers. `ColumnDiffApp` parses CLI arguments and wires dependencies.

## Data Flow
1. CLI receives three paths (A, B, output) and delegates to `ColumnProcessingPipeline`.
2. `ColumnInputProvider` reads each file, `ColumnParser` trims whitespace, infers `ValueType`, and emits `SeriesData`.
3. `SeriesComparatorFactory` selects a `SeriesComparator` based on `SeriesDescriptor::type()` and executes the comparison.
4. `SeriesDiff` returns to the boundary where `PlainTextOutputFormatter` and `TxtDiffOutputWriter` persist results.

## Extension Checklist
### Adding a New Value Type
1. Register a prefix in `valueTypeFromHeader` and extend `toString` with the new enum value.
2. Implement a `SeriesComparator` specialized for the type (e.g., `DateSeriesComparator`).
3. Update `SeriesComparatorFactory::create` to build the comparator (consider refactoring to a registry when multiple custom comparators exist).
4. Add unit tests covering parsing, comparator behavior, and pipeline integration.

### Supporting Alternative Inputs/Outputs
- Implement a new `ColumnParser`/`ColumnInputProvider` pair (e.g., CSV parser) that still returns `SeriesData`.
- Provide a matching writer by subclassing `DiffOutputWriter` (e.g., JSON output) while reusing `SeriesDiff`.
- Register the new components in a factory or wire them in an alternate `main` if the build needs multiple front ends.

## Naming & Organization
- Use `Series*` for core domain classes, `ValueType` for the inferred type enum, and reserve `Column*` prefixes for I/O adapters.
- Group headers under `include/diff_compare/core|io|app/` and mirror the structure in `src/core|io|app/` so each component keeps its layer-local dependencies obvious.
- Keep executable wiring (CLI, main) in `src/app/` and high-level tests under `test/`, mirroring the modules they exercise.

## Testing Strategy
- Unit tests in `test/test_diff_compare.cpp` demonstrate end-to-end usage through the pipeline. New components should receive suite-specific tests (`TEST(NewComparator, Scenario)`).
- Run `cmake --preset diff_compare`, `cmake --build build`, and `ctest --preset diff_compare` to validate changes before committing.
