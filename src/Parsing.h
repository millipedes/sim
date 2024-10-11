#pragma once

#include <optional>
#include <string>
#include <tl/expected.hpp>
#include <vector>

using Strings = std::vector<std::string>;

struct Command {
  std::string name;
  std::optional<Strings> arguments;
  // TODO: add label addresses
  std::optional<uint64_t> address;

  Command() : name(""), arguments(std::nullopt), address(std::nullopt) {}
  Command(std::string name,
      std::optional<Strings> arguments,
      std::optional<uint64_t> address)
    : name(name),
      arguments(arguments),
      address(address) {}

  auto operator==(const Command& other) const -> bool = default;
};
using Commands = std::vector<Command>;

auto file_to_string(const std::string& file_name)
  -> tl::expected<std::string, std::string>;

using ResultCommands = tl::expected<Commands, std::string>;
auto parse_json(const std::string& file_name) -> ResultCommands;
