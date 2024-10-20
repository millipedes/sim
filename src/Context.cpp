#include "Context.h"

#include <iostream>
#include <functional>
#include <memory>
#include <ranges>

// We want to be able to handle/give context to errors when running sedim
// scripts
using ResultContext = tl::expected<Context, std::string>;

auto append_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("append_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("append_function: append expects 1 argument");
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream = *context.operations_stream
      + std::string(nl) + (*command.arguments)[0];
  }
  return context;
}

auto change_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("change_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("change_function: change expects 1 argument");
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream = (*command.arguments)[0];
  }
  return context;
}

auto delete_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cout << "delete_function: the delete command does not take arguments "
      "ignoring them" << std::endl;
  }
  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream = std::nullopt;
  }
  return context;
}

auto insert_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("insert_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("insert_function: insert expects 1 argument");
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream = (*command.arguments)[0]
      + std::string(nl) + *context.operations_stream;
  }
  return context;
}

auto execute_function(Context context, const Command& command) -> ResultContext {
#ifndef __linux__
  return tl::make_unexpected("execute_function: command line execution only "
      "supported for linux");
#else
  if (command.arguments) {
    std::cout << "execute_function: the execute command does not take arguments "
      "ignoring them" << std::endl;
  }

  std::array<char, 128> buffer;
  std::string result;
  auto full_command = *context.operations_stream;


  if (command.address && context.cycle == *command.address) {
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(full_command.c_str(), "r"), pclose);
    if (!pipe) {
      return tl::make_unexpected("execute_function: popen() failed");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get())) {
      result += buffer.data();
    }
    context.operations_stream = result;
  } else if (!command.address) {
    std::cout << "execute_function: warning: whole input file is being "
      "executed, just write a shell script?" << std::endl;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(full_command.c_str(), "r"), pclose);
    if (!pipe) {
      return tl::make_unexpected("execute_function: popen() failed");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get())) {
      result += buffer.data();
    }
    context.operations_stream = result;
  }
  return context;
#endif
}

auto add_to_static_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "add_to_static_function: the add_to_static command does not "
      "take arguments ignoring them" << std::endl;
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    context.static_stream = context.operations_stream;
  }
  return context;
}

auto nl_add_to_static_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "add_to_static_function: the add_to_static command does not "
      "take arguments ignoring them" << std::endl;
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    context.static_stream = context.static_stream
      ? *context.static_stream + std::string(nl) + *context.operations_stream
      : std::string(nl) + *context.operations_stream;
  }
  return context;
}

auto replace_operation_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "replace_operation_function: the replace_operation command does not "
      "take arguments ignoring them" << std::endl;
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream = context.static_stream
      ? context.static_stream : "";
  }
  return context;
}

auto nl_replace_operation_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "nl_replace_operation_function: the nl_replace_operation command does not "
      "take arguments ignoring them" << std::endl;
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream = *context.operations_stream + std::string(nl);
    if (context.static_stream) {
      context.operations_stream = *context.operations_stream + *context.static_stream;
    }
  }
  return context;
}

auto unamb_operations_stream(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "print_operations_function: the print_operations command does not "
      "take arguments ignoring them" << std::endl;
  }
  if (!context.operations_stream) {
    return tl::make_unexpected("unamb_operations_stream: operations_stream holds "
        "no value");
  }
  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream =
      (*context.operations_stream + std::string("$")
       + std::string(nl) + *context.operations_stream);
  }
  return context;
}

auto translate_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("translate_function: no arguments provided");
  } else if (command.arguments->size() != 2) {
    return tl::make_unexpected("translate_function: translate expects 1 argument");
  }

  size_t pos = 0;
  while ((pos = (*context.operations_stream)
        .find((*command.arguments)[0])) != std::string::npos) {
    (*context.operations_stream)
      .replace(pos, (*command.arguments)[0].length(), (*command.arguments)[1]);
    pos += (*command.arguments)[1].length();
  }

  return context;
}

using SemanticFunc = std::function<ResultContext(Context, const Command&)>;
using CommandSemanticUMap = std::unordered_map<std::string, SemanticFunc>;
static inline auto control_flow_map = CommandSemanticUMap {
  {"a",                       append_function},
  {"append",                  append_function},
  {"c",                       change_function},
  {"change",                  change_function},
  {"d",                       delete_function},
  {"delete",                  delete_function},
  {"i",                       insert_function},
  {"insert",                  insert_function},
  {"e",                       execute_function},
  {"execute",                 execute_function},
  {"h",                       add_to_static_function},
  {"add_to_static",           add_to_static_function},
  {"H",                       nl_add_to_static_function},
  {"nl_add_to_static",        nl_add_to_static_function},
  {"g",                       replace_operation_function},
  {"replace_operation",       replace_operation_function},
  {"G",                       nl_replace_operation_function},
  {"nl_replace_operation",    nl_replace_operation_function},
  {"l",                       unamb_operations_stream},
  {"unamb_operations_stream", unamb_operations_stream},
  {"y",                       translate_function},
  {"translate",               translate_function},
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
        if ((command.address && *command.address == context.cycle) || !command.address) {
          break;
        }
      } else if (control_flow_map.contains(command.name)) {
        auto maybe_context = control_flow_map.at(command.name)(std::move(context), command);
        if (!maybe_context) {
          throw std::runtime_error(std::string("execute: unable to execute command: ")
              + maybe_context.error());
        }
        context = std::move(maybe_context.value());
      } else {
        throw std::runtime_error(std::string("execute: no command with name: ")
            + command.name);
      }
    }
    if (context.operations_stream) {
      result += (*context.operations_stream + std::string(nl));
    }
    context.stream.remove_prefix(pos + 1);
  }
  return result;
}
