#include "optional.h"

#include "test-object.h"

#include <gtest/gtest.h>

namespace {

struct only_movable : test_object {
  using test_object::test_object;
  only_movable(const only_movable&) = delete;
  only_movable& operator=(const only_movable&) = delete;

  only_movable(only_movable&& other) noexcept : test_object(std::move(other)) {}

  only_movable& operator=(only_movable&& other) noexcept {
    static_cast<test_object&>(*this) = std::move(other);
    return *this;
  }
};

class optional_test : public ::testing::Test {
protected:
  test_object::no_new_instances_guard instances_guard;
};

} // namespace

const nullopt_t* get_nullopt_ptr() noexcept {
  return &nullopt;
}

const in_place_t* get_in_place_ptr() noexcept {
  return &in_place;
}

TEST_F(optional_test, default_ctor) {
  optional<test_object> a;
  EXPECT_FALSE(static_cast<bool>(a));
}

TEST_F(optional_test, default_ctor_no_instances) {
  optional<test_object> a;
  EXPECT_FALSE(static_cast<bool>(a));
  instances_guard.expect_no_instances();
}

TEST_F(optional_test, value_ctor) {
  optional<int> a(42);
  EXPECT_TRUE(static_cast<bool>(a));
}

TEST_F(optional_test, dereference) {
  optional<int> a(42);
  EXPECT_EQ(42, *a);
  EXPECT_EQ(42, *std::as_const(a));
  EXPECT_EQ(42, *std::move(a));
  EXPECT_EQ(42, *std::move(std::as_const(a)));
}

TEST_F(optional_test, member_access) {
  optional<test_object> a(42);
  EXPECT_EQ(42, a->operator int());
  EXPECT_EQ(42, std::as_const(a)->operator int());
}

TEST_F(optional_test, reset) {
  optional<test_object> a(42);
  EXPECT_TRUE(static_cast<bool>(a));
  a.reset();
  EXPECT_FALSE(static_cast<bool>(a));
  instances_guard.expect_no_instances();
}

TEST_F(optional_test, dtor) {
  optional<test_object> a(42);
  EXPECT_TRUE(static_cast<bool>(a));
  EXPECT_EQ(42, *a);
}

TEST_F(optional_test, copy_ctor) {
  optional<test_object> a(42);
  optional<test_object> b = a;
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST_F(optional_test, copy_ctor_empty) {
  optional<test_object> a;
  optional<test_object> b = a;
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST_F(optional_test, move_ctor) {
  optional<only_movable> a(42);
  optional<only_movable> b = std::move(a);
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST_F(optional_test, move_ctor_empty) {
  optional<test_object> a;
  optional<test_object> b = std::move(a);
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST_F(optional_test, copy_assignment_empty_empty) {
  optional<test_object> a, b;
  b = a;
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST_F(optional_test, copy_assignment_to_empty) {
  optional<test_object> a(42), b;
  b = a;
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST_F(optional_test, copy_assignment_from_empty) {
  optional<test_object> a, b(42);
  b = a;
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST_F(optional_test, copy_assignment) {
  optional<test_object> a(42), b(41);
  b = a;
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST_F(optional_test, move_assignment_empty_empty) {
  optional<only_movable> a, b;
  b = std::move(a);
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST_F(optional_test, move_assignment_to_empty) {
  optional<only_movable> a(42), b;
  b = std::move(a);
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST_F(optional_test, move_assignment_from_empty) {
  optional<only_movable> a, b(42);
  b = std::move(a);
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST_F(optional_test, move_assignment) {
  optional<only_movable> a(42), b(41);
  b = std::move(a);
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST_F(optional_test, nullopt_ctor) {
  optional<test_object> a = nullopt;
  EXPECT_FALSE(static_cast<bool>(a));
  instances_guard.expect_no_instances();
}

TEST_F(optional_test, nullopt_assignment) {
  optional<test_object> a(42);
  a = nullopt;
  EXPECT_FALSE(static_cast<bool>(a));
  EXPECT_TRUE(noexcept(a = nullopt));
  instances_guard.expect_no_instances();
}

TEST_F(optional_test, empty_ctor) {
  optional<test_object> a = {};
  EXPECT_FALSE(static_cast<bool>(a));
  instances_guard.expect_no_instances();
}

TEST_F(optional_test, empty_assignment) {
  std::optional<test_object> a(42);
  a = {};
  EXPECT_FALSE(static_cast<bool>(a));
  instances_guard.expect_no_instances();
}

TEST_F(optional_test, swap_non_empty) {
  std::optional<test_object> a(42);
  std::optional<test_object> b(55);

  swap(a, b);

  EXPECT_EQ(55, *a);
  EXPECT_EQ(42, *b);
}

TEST_F(optional_test, swap_empty_right) {
  std::optional<test_object> a(42);
  std::optional<test_object> b;

  swap(a, b);

  EXPECT_FALSE(a);
  EXPECT_EQ(42, *b);
}

TEST_F(optional_test, swap_empty_left) {
  std::optional<test_object> a;
  std::optional<test_object> b(55);

  swap(a, b);

  EXPECT_EQ(55, *a);
  EXPECT_FALSE(b);
}

TEST_F(optional_test, swap_empty_both) {
  std::optional<test_object> a;
  std::optional<test_object> b;

  swap(a, b);

  EXPECT_FALSE(a);
  EXPECT_FALSE(b);
}

namespace {

struct custom_swap {
  explicit custom_swap(int value) noexcept : value(value) {}

  friend void swap(custom_swap& lhs, custom_swap& rhs) noexcept {
    std::swap(lhs.value, rhs.value);
    ++lhs.value;
    ++rhs.value;
  }

  int value;
};

} // namespace

TEST_F(optional_test, swap_custom) {
  std::optional<custom_swap> a(42);
  std::optional<custom_swap> b(55);

  swap(a, b);

  EXPECT_EQ(56, a->value);
  EXPECT_EQ(43, b->value);
}

TEST_F(optional_test, swap_empty_custom) {
  std::optional<custom_swap> a(42);
  std::optional<custom_swap> b;

  swap(a, b);

  EXPECT_FALSE(a);
  EXPECT_EQ(42, b->value);
}

namespace {

struct non_default_constructor {
  non_default_constructor(int, int, int, std::unique_ptr<int>) {}
};

} // namespace

TEST_F(optional_test, in_place_ctor) {
  optional<non_default_constructor> a(in_place, 1, 2, 3, std::unique_ptr<int>());
  EXPECT_TRUE(static_cast<bool>(a));
}

TEST_F(optional_test, emplace) {
  optional<non_default_constructor> a;
  a.emplace(1, 2, 3, std::unique_ptr<int>());
  EXPECT_TRUE(static_cast<bool>(a));
}

namespace {

struct throw_in_ctor {
  struct exception : std::exception {
    using std::exception::exception;
  };

  throw_in_ctor(int, int) {
    if (enable_throw) {
      throw exception();
    }
  }

  inline static bool enable_throw = false;
};

} // namespace

TEST_F(optional_test, emplace_throw) {
  optional<throw_in_ctor> a(in_place, 1, 2);
  throw_in_ctor::enable_throw = true;
  EXPECT_THROW(a.emplace(3, 4), throw_in_ctor::exception);
  EXPECT_FALSE(static_cast<bool>(a));
}

TEST_F(optional_test, comparison_non_empty_and_non_empty) {
  optional<int> a(41), b(42);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(a > b);
  EXPECT_FALSE(a >= b);

  EXPECT_TRUE(a == a);
  EXPECT_FALSE(a != a);
  EXPECT_FALSE(a < a);
  EXPECT_TRUE(a <= a);
  EXPECT_FALSE(a > a);
  EXPECT_TRUE(a >= a);

  EXPECT_FALSE(b == a);
  EXPECT_TRUE(b != a);
  EXPECT_FALSE(b < a);
  EXPECT_FALSE(b <= a);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(b >= a);
}

TEST_F(optional_test, comparison_non_empty_and_empty) {
  optional<int> a(41), b;
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
  EXPECT_FALSE(a < b);
  EXPECT_FALSE(a <= b);
  EXPECT_TRUE(a > b);
  EXPECT_TRUE(a >= b);

  EXPECT_FALSE(b == a);
  EXPECT_TRUE(b != a);
  EXPECT_TRUE(b < a);
  EXPECT_TRUE(b <= a);
  EXPECT_FALSE(b > a);
  EXPECT_FALSE(b >= a);
}

TEST_F(optional_test, comparison_empty_and_empty) {
  optional<int> a, b;
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
  EXPECT_FALSE(a < b);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(a > b);
  EXPECT_TRUE(a >= b);

  EXPECT_TRUE(a == a);
  EXPECT_FALSE(a != a);
  EXPECT_FALSE(a < a);
  EXPECT_TRUE(a <= a);
  EXPECT_FALSE(a > a);
  EXPECT_TRUE(a >= a);

  EXPECT_TRUE(b == a);
  EXPECT_FALSE(b != a);
  EXPECT_FALSE(b < a);
  EXPECT_TRUE(b <= a);
  EXPECT_FALSE(b > a);
  EXPECT_TRUE(b >= a);
}
