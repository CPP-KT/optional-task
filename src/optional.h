#pragma once

struct nullopt_t;

struct in_place_t;

template <typename T>
class optional {
public:
  constexpr optional() noexcept;
  constexpr optional(nullopt_t) noexcept;

  constexpr optional(const optional&);
  constexpr optional(optional&&);

  optional& operator=(const optional&);
  optional& operator=(optional&&);

  constexpr optional(T value);

  template <typename... Args>
  explicit constexpr optional(in_place_t, Args&&... args);

  optional& operator=(nullopt_t) noexcept;

  constexpr explicit operator bool() const noexcept;

  constexpr T& operator*() noexcept;
  constexpr const T& operator*() const noexcept;

  constexpr T* operator->() noexcept;
  constexpr const T* operator->() const noexcept;

  template <typename... Args>
  void emplace(Args&&... args);

  void reset();
};

template <typename T>
constexpr bool operator==(const optional<T>& a, const optional<T>& b);

template <typename T>
constexpr bool operator!=(const optional<T>& a, const optional<T>& b);

template <typename T>
constexpr bool operator<(const optional<T>& a, const optional<T>& b);

template <typename T>
constexpr bool operator<=(const optional<T>& a, const optional<T>& b);

template <typename T>
constexpr bool operator>(const optional<T>& a, const optional<T>& b);

template <typename T>
constexpr bool operator>=(const optional<T>& a, const optional<T>& b);
