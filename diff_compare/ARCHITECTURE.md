# Architecture Overview

## Layered Design
- **Series Core**: Pure domain model (`SeriesDescriptor`, `SeriesData`, `SeriesDiff`) plus comparison strategies. This layer is unaware of files or "columns" and only reasons about homogeneous series of string values. Comparators implement `SeriesComparator` and return `SeriesDiff` objects.
- **Column I/O Boundary**: Generic `SeriesInputProvider`/`SeriesOutputWriter` contracts decouple the core from transport concerns; the default text stack (`ColumnParser`, `TxtColumnInputProvider`, `TxtDiffOutputWriter`) adapts them to newline-delimited columns.
- **Application Shell**: `ColumnProcessingPipeline` orchestrates providers, comparator factories, and writers. `ColumnDiffApp` parses CLI arguments and wires dependencies.

## Data Flow
1. CLI receives three paths (A, B, output) and delegates to `ColumnProcessingPipeline`.
2. A `SeriesInputProvider` reads each source; the default `TxtColumnInputProvider` uses `ColumnParser` to trim whitespace, infer `ValueType`, and emit `SeriesData`.
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
