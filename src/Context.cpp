#include "Context.h"

#include <functional>
#include <ranges>

// We want to be able to handle/give context to errors when running sedim
// scripts
using ResultContext = tl::expected<Context, std::string>;

auto delete_function(Context context, const Command& command) -> ResultContext {
  if (command.address && context.cycle == command.address) {
    context.operations_stream = std::nullopt;
  } else if (!command.address) {
    context.operations_stream = std::nullopt;
  }
  return context;
}

auto append_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("append_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("append_function: appened expects 1 argument");
  }

  if (command.address && context.cycle == command.address) {
    context.operations_stream = context.operations_stream.value()
      + std::string(nl) + command.arguments.value()[0];
  } else if (!command.address) {
    context.operations_stream = context.operations_stream.value()
      + std::string(nl) + command.arguments.value()[0];
  }
  return context;
}

using SemanticFunc = std::function<ResultContext(Context, const Command&)>;
using CommandSemanticUMap = std::unordered_map<std::string, SemanticFunc>;
static inline CommandSemanticUMap control_flow_map = CommandSemanticUMap {
  {"d",      delete_function},
  {"delete", delete_function},
  {"a",      append_function},
  {"append", append_function},
};

auto execute(const std::string& input,
    const std::string& command_json) -> std::string {
  std::string result;

  auto context = Context(input);
  auto maybe_commands = parse_json(command_json);
  if (!maybe_commands) {
    throw std::runtime_error(std::string("execute: unable to parse json: ")
        + maybe_commands.error());
  }
  context.commands = maybe_commands.value();

  size_t pos;
  while ((pos = context.stream.find(nl)) != std::string_view::npos) {
    context.operations_stream = context.stream.substr(0, pos);
    context.cycle++;
    for (const auto& command : context.commands) {
      if (command.name == "d" || command.name == "delete") {
        context = control_flow_map.at(command.name)(std::move(context), command).value();
        // special case where none of the rest of the commands execute
        if ((command.address && command.address == context.cycle) || !command.address) {
          break;
        }
      } else if (control_flow_map.contains(command.name)) {
        context = control_flow_map.at(command.name)(std::move(context), command).value();
      } else {
        throw std::runtime_error(std::string("execute: no command with name: ")
            + command.name);
      }
    }
    if (context.operations_stream) {
      result += (context.operations_stream.value() + std::string(nl));
    }
    context.stream.remove_prefix(pos + 1);
  }
  return result;
}
