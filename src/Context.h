#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <tl/expected.hpp>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Parsing.h"

#if defined(_WIN32) || defined(_WIN64)
  static constexpr auto nl = "\r\n";
#else
  static constexpr auto nl = "\n";
#endif

auto execute(const std::string& input,
    const std::string& command_json) -> std::string;

struct Context {
  std::string_view stream;
  std::optional<std::string> operations_stream;
  Strings static_stream;
  Commands commands;
  uint64_t cycle;

  Context(const std::string& stream) 
    : stream(stream),
      operations_stream(std::nullopt),
      static_stream(Strings()),
      commands(Commands()),
      cycle(0) {}

  // destructor
  ~Context() = default;

  // copy constructor
  Context(const Context& other)
    : stream(other.stream),
      operations_stream(other.operations_stream),
      static_stream(other.static_stream),
      commands(other.commands),
      cycle(other.cycle) {}

  // copy assignment
  auto operator=(const Context& other) -> Context& {
    if (this != &other) {
      stream = other.stream;
      operations_stream = other.operations_stream;
      static_stream = other.static_stream;
      commands = other.commands;
      cycle = other.cycle;
    }
    return *this;
  }

  // move constructor
  Context(Context&& other) noexcept
    : stream(other.stream),
      operations_stream(std::move(other.operations_stream)),
      static_stream(std::move(other.static_stream)),
      commands(std::move(other.commands)),
      cycle(other.cycle) {}

  // move assignment
  auto operator=(Context&& other) noexcept -> Context& {
    if (this != &other) {
      stream = other.stream;
      operations_stream = std::move(other.operations_stream);
      static_stream = std::move(other.static_stream);
      commands = std::move(other.commands);
      cycle = other.cycle;
    }
    return *this;
  }
};
