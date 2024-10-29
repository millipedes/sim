#pragma once

#if defined(__linux__)
#include <cstdio>
#endif
#include <fstream>
#include <functional>
#include <optional>
#include <string>
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

using TextToCommands = std::function<ResultCommands(const std::string&)>;

auto execute_from_files(const std::string& input_file,
    const std::string& command_file) -> std::string;
auto execute(const std::string& input_text, const std::string& command_text,
    const std::optional<std::string>& file_name = std::nullopt,
    const TextToCommands& text_to_commands = parse_json) -> std::string;

struct Context {
  // optional for testing purposes, we want to be calling execute over
  // execute_from_files, want to rely as little as possible on file io as it
  // makes stuff more complicated.
  std::pair<std::optional<std::string>, std::string> file_stream;
  std::unordered_map<std::string, std::fstream> stream_map;
  std::optional<std::string> operations_stream;
  std::optional<std::string> static_stream;
  Commands commands;
  uint64_t cycle;

  Context(const std::pair<std::optional<std::string>, std::string>& file_stream)
    : file_stream(file_stream),
      stream_map(std::unordered_map<std::string, std::fstream>()),
      operations_stream(std::nullopt),
      static_stream(std::nullopt),
      commands(Commands()),
      cycle(0) {}

  // destructor
  ~Context() {
    for (auto& [name, stream] : stream_map) {
      stream.close();
    }
  }

  // copy constructor
  // N.B. other is not const because tellg can put it into an error state :(
  // But with the way the program is written, you shouldn't really be copying a
  // Context anyways
  Context(Context& other)
    : file_stream(other.file_stream),
      stream_map(std::unordered_map<std::string, std::fstream>()),
      operations_stream(other.operations_stream),
      static_stream(other.static_stream),
      commands(other.commands),
      cycle(other.cycle) {
        for (auto& [name, stream] : other.stream_map) {
          stream_map[name] = std::fstream(name);
          if (!stream_map[name]) {
            throw std::runtime_error(std::string("Copy Constructor: Unable "
                  "to open file: ") + name);
          }
          stream_map[name].seekg(stream.tellg());
        }
      }

  // copy assignment
  // N.B. other is not const because tellg can put it into an error state :(
  // But with the way the program is written, you shouldn't really be copying a
  // Context anyways
  auto operator=(Context& other) -> Context& {
    if (this != &other) {
      file_stream = other.file_stream;
      for (auto& [name, stream] : other.stream_map) {
        stream_map[name] = std::fstream(name);
        if (!stream_map[name]) {
          throw std::runtime_error(std::string("Copy Assignment: Unable "
                "to open file: ") + name);
        }
        stream_map[name].seekg(stream.tellg());
      }
      operations_stream = other.operations_stream;
      static_stream = other.static_stream;
      commands = other.commands;
      cycle = other.cycle;
    }
    return *this;
  }

  // move constructor
  Context(Context&& other) noexcept
    : file_stream(other.file_stream),
      stream_map(std::move(other.stream_map)),
      operations_stream(std::move(other.operations_stream)),
      static_stream(std::move(other.static_stream)),
      commands(std::move(other.commands)),
      cycle(other.cycle) {}

  // move assignment
  auto operator=(Context&& other) noexcept -> Context& {
    if (this != &other) {
      file_stream = other.file_stream;
      stream_map = std::move(other.stream_map);
      operations_stream = std::move(other.operations_stream);
      static_stream = std::move(other.static_stream);
      commands = std::move(other.commands);
      cycle = other.cycle;
    }
    return *this;
  }
};
