#ifndef TYPES_UNCOPYABLE_H
#define TYPES_UNCOPYABLE_H
#pragma once
namespace cpplox::Types {

struct Uncopyable {
  explicit Uncopyable() = default;
  virtual ~Uncopyable() = default;
  Uncopyable(const Uncopyable&) = delete;
  auto operator=(const Uncopyable&) -> Uncopyable& = delete;
  Uncopyable(Uncopyable&&) = delete;
  auto operator=(Uncopyable &&) -> Uncopyable& = delete;
};  // struct Uncopyable

}  // namespace cpplox::Types
#endif  // TYPES_UNCOPYABLE_H