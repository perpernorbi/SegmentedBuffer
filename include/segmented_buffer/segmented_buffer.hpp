#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <span>
#include <tuple>
#include <type_traits>

namespace segmented_buffer {
namespace detail {
template <class T, class... Ts>
inline constexpr bool contains_v = (std::is_same_v<T, Ts> || ...);

template <class T, class... Ts>
struct index_of;

template <class T, class... Ts>
struct index_of<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

template <class T, class U, class... Ts>
struct index_of<T, U, Ts...>
    : std::integral_constant<std::size_t, 1 + index_of<T, Ts...>::value> {};

template <typename...>
inline constexpr bool unique_types_v = true;

template <typename T, typename... Rest>
inline constexpr bool unique_types_v<T, Rest...> =
  (!std::is_same_v<T, Rest> && ...) && unique_types_v<Rest...>;

template <typename... Tags>
concept UniqueTags = unique_types_v<Tags...>;

// ---------- option B: seg<Tag>(n) ----------
template <class Tag>
struct SegmentSize {
  using tag = Tag;
  explicit SegmentSize(const std::size_t n) : n(n) {}
  std::size_t n;
};

template <class T>
concept BufferElement =
  std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>;

} // namespace detail

template <class Tag>
constexpr auto Segment(const std::size_t n) noexcept {
  return detail::SegmentSize<Tag>{n};
}

template <class Underlier, class... Tags>
  requires detail::BufferElement<Underlier> && detail::UniqueTags<Tags...>
class Buffer {
public:
  static constexpr std::size_t N = sizeof...(Tags);

  template <class... Specs>
    requires std::is_same_v<std::tuple<Specs...>,
                            std::tuple<detail::SegmentSize<Tags>...>>
  explicit Buffer(const Specs... specs) {
    std::size_t cur = 0;
    std::size_t i = 0;
    auto add = [&](std::size_t count) {
      cur += count;
      ends_[i++] = cur;
    };
    (add(specs.n), ...);

    data_ = std::make_unique<Underlier[]>(N == 0 ? 0 : ends_[N - 1]);
  }

  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;
  Buffer(Buffer &&) noexcept = default;
  Buffer &operator=(Buffer &&) noexcept = default;

  template <class Tag>
    requires detail::contains_v<Tag, Tags...>
  [[nodiscard]] std::span<Underlier> get() noexcept {
    constexpr std::size_t i = detail::index_of<Tag, Tags...>::value;
    const std::size_t end = ends_[i];
    const std::size_t start = (i == 0) ? 0 : ends_[i - 1];
    return {data_.get() + start, end - start};
  }

  template <class Tag>
    requires detail::contains_v<Tag, Tags...>
  [[nodiscard]] std::span<const Underlier> get() const noexcept {
    constexpr std::size_t i = detail::index_of<Tag, Tags...>::value;
    const std::size_t end = ends_[i];
    const std::size_t start = (i == 0) ? 0 : ends_[i - 1];
    return {data_.get() + start, end - start};
  }

  [[nodiscard]] std::size_t total_size() const noexcept {
    return N == 0 ? 0 : ends_[N - 1];
  }

private:
  std::array<std::size_t, N> ends_{}; // boundaries (prefix sums)
  std::unique_ptr<Underlier[]> data_{};
};

template <class Underlier, class... Specs>
auto MakeSegmentedBuffer(Specs... specs) {
  return Buffer<Underlier, typename Specs::tag...>(specs...);
}
} // namespace segmented_buffer
