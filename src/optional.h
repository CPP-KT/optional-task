#pragma once

#include <compare>

struct nullopt_t;

struct in_place_t;

template <typename T>
class optional {
public:
  constexpr optional() noexcept;
  constexpr optional(nullopt_t) noexcept;

  constexpr optional(const optional&);
  constexpr optional(optional&&);

  constexpr optional& operator=(const optional&);
  constexpr optional& operator=(optional&&);

  constexpr optional(T value);

  template <typename... Args>
  explicit constexpr optional(in_place_t, Args&&... args);

  constexpr optional& operator=(nullopt_t) noexcept;

  friend constexpr void swap(optional& lhs, optional& rhs);

  constexpr bool has_value() const noexcept;
  constexpr explicit operator bool() const noexcept;

  constexpr T& operator*() & noexcept;
  constexpr const T& operator*() const& noexcept;
  constexpr T&& operator*() && noexcept;
  constexpr const T&& operator*() const&& noexcept;

  constexpr T* operator->() noexcept;
  constexpr const T* operator->() const noexcept;

  template <typename... Args>
  constexpr void emplace(Args&&... args);

  constexpr void reset() noexcept;
};

template <typename T>
constexpr bool operator==(const optional<T>& lhs, const optional<T>& rhs);

template <typename T>
constexpr bool operator!=(const optional<T>& lhs, const optional<T>& rhs);

template <typename T>
constexpr bool operator<(const optional<T>& lhs, const optional<T>& rhs);

template <typename T>
constexpr bool operator<=(const optional<T>& lhs, const optional<T>& rhs);

template <typename T>
constexpr bool operator>(const optional<T>& lhs, const optional<T>& rhs);

template <typename T>
constexpr bool operator>=(const optional<T>& lhs, const optional<T>& rhs);

template <class T>
constexpr std::compare_three_way_result_t<T> operator<=>(const optional<T>& lhs, const optional<T>& rhs);
