# SegmentedBuffer

`SegmentedBuffer` is a small, header-only C++20 utility for managing multiple
fixed-size contiguous segments in a **single allocation**, accessed by tag
types and exposed as `std::span`.

The typical use case is a class or struct that conceptually owns several
vectors whose sizes:

- are **not known at compile time**
- are **known at construction time**
- **never change** afterward

Instead of allocating and storing multiple `std::vector`s, `SegmentedBuffer`
allocates once and provides lightweight views into the underlying storage.

---

## Motivation

It is common to see classes like this:

```cpp
struct Data {
  std::vector<double> foo;
  std::vector<double> bar;
};
```

where each vector’s size is fixed after construction.

This approach has drawbacks:
- multiple heap allocations
- more memory fragmentation
- extra metadata per vector
- harder to reason about memory layout

`SegmentedBuffer` addresses this by:
- performing **exactly one allocation**
- storing all segments contiguously
- providing **type-safe access** via tags
- returning `std::span`, not owning containers

---

## Basic usage

Define empty tag types to identify segments:

```cpp
struct Foo {};
struct Bar {};
```

Construct a buffer using the factory function and `Segment<Tag>(size)` helpers:

```cpp
#include <segmented_buffer/segmented_buffer.hpp>

auto buffer = segmented_buffer::MakeSegmentedBuffer<double>(
  segmented_buffer::Segment<Foo>(10),
  segmented_buffer::Segment<Bar>(20)
);
```

Access segments by tag:

```cpp
std::span<double> foo  = buffer.get<Foo>();
std::span<double> bar = buffer.get<Bar>();
```

Each call to `get<Tag>()` returns a `std::span` covering that segment.

---

## Design properties

- Single allocation
- Contiguous layout
- Fixed sizes after construction
- No resizing
- No runtime lookup
- No virtual dispatch
- Zero overhead abstractions

All errors related to:
- missing segments
- duplicated tags
- incorrect construction

are detected at compile time.

---

## API overview

### Segment declaration

```cpp
template <class Tag>
segment_size<Tag> Segment(std::size_t size);
```

### Factory

```cpp
template <class T, class... Specs>
auto MakeSegmentedBuffer(Specs... specs);
```

### Access

```cpp
template <class Tag>
[[nodiscard]] std::span<T> get() noexcept;

template <class Tag>
[[nodiscard]] std::span<const T> get() const noexcept;
```

### Utilities

```cpp
[[nodiscard]] std::size_t total_size() const noexcept;
```

---

## Constraints and assumptions

- All segments store the same element type
- The element type must be trivially copyable and trivially destructible
- Storage is allocated as an array (`make_unique<T[]>(n)`)
- For arithmetic types (e.g. `int`, `double`), elements are **uninitialized**
- Segment sizes are fixed after construction
- This is a storage primitive, not a container replacement

If you need:
- resizing
- per-segment element types
- complex lifetimes

this is likely not the right abstraction.

---

## Header-only

The library is header-only.

```cpp
#include <segmented_buffer/segmented_buffer.hpp>
```

No compilation unit is required.

---

## Testing

Tests are written using GoogleTest / GoogleMock and are built via CMake using
`FetchContent`.

To build and run tests:

```bash
cmake -S . -B build -DSEGMENTED_BUFFER_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

---

## C++ standard

Requires **C++20**.

---

## License

MIT
