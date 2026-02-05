#include <segmented_buffer/segmented_buffer.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <span>

struct Foo {};
struct Bar {};

TEST(SegmentedBuffer, BasicSizesAndAccess) {
  auto b = segmented_buffer::MakeSegmentedBuffer<double>(
    segmented_buffer::Segment<Foo>(10), segmented_buffer::Segment<Bar>(20));

  auto levels = b.get<Foo>();
  auto results = b.get<Bar>();

  EXPECT_EQ(levels.size(), 10u);
  EXPECT_EQ(results.size(), 20u);
  EXPECT_EQ(b.total_size(), 30u);

  levels[0] = 1.0;
  levels[9] = 1.9;
  results[0] = 2.0;
  results[19] = 2.95;
  EXPECT_EQ(levels[0], 1.0);
  EXPECT_EQ(levels[9], 1.9);
  EXPECT_EQ(results[0], 2.0);
  EXPECT_EQ(results[19], 2.95);
}

// Example usage inside a class
class ClassWithSegmentedBuffer {
  struct One {};
  struct Two {};
  segmented_buffer::Buffer<double, One, Two> b;

public:
  explicit ClassWithSegmentedBuffer(const std::size_t a, const std::size_t b)
      : b(segmented_buffer::Segment<One>(a),
          segmented_buffer::Segment<Two>(b)) {}
};

TEST(SegmentedBuffer, AsClassMemeber) {
  ClassWithSegmentedBuffer cls(7, 8);
}