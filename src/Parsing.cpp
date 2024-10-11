#include "Parsing.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::ordered_json;

auto file_to_string(const std::string& file_name)
  -> tl::expected<std::string, std::string> {
  auto file_stream = std::ifstream(file_name);
  if (!file_stream) {
    return tl::make_unexpected(
        std::string("file_to_string: unable to read file with name: ") + file_name);
  }

  std::ostringstream ss;
  ss << file_stream.rdbuf();
  return ss.str();
}

auto parse_json(const std::string& contents) -> ResultCommands {
  Commands result;
  auto json_object = json::parse(contents);
  for (auto& [key, value] : json_object.items()) {
    result.push_back(Command());
    if (value.is_object()) {
      result[result.size() - 1].name = key;
      for (auto& [sub_key, sub_value] : value.items()) {
        if (sub_key == "address" && sub_value.is_number()) {
          result[result.size() - 1].address = sub_value;
        } else if (sub_key == "arguments" && sub_value.is_array()) {
          result[result.size() - 1].arguments = Strings();
          for (auto& argument : sub_value) {
            result[result.size() - 1].arguments->push_back(argument);
          }
        } else {
          return tl::make_unexpected(std::string("parse_json: key: ")
              + key + std::string(" is either not supported or has a value "
                "which is not supported under the current schema"));
        }
      }
    } else {
      return tl::make_unexpected(std::string("parse_json: command: ")
          + key + std::string(" is not a json object, see the documentation"));
    }
  }
  return result;
}
