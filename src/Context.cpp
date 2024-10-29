#include "Context.h"

#include <iostream>
#include <memory>
#include <ranges>
#include <regex>
#include <sstream>

#include "Version.h"

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
    std::cerr << "delete_function: warning: the delete command does not take arguments "
      "ignoring them" << std::endl;
  }
  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream = std::nullopt;
      context.current_command = context.commands.size();
  }
  return context;
}

auto delete_restart_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "delete_function: warning: the delete command does not take arguments "
      "ignoring them" << std::endl;
  }
  size_t pos = 0;
  if (command.address && context.cycle == *command.address || !command.address) {
    if ((pos = context.operations_stream->find(nl)) != std::string::npos) {
      context.operations_stream = context.operations_stream->erase(0, pos + 1);
      context.current_command = 0;
    } else {
      context.operations_stream = std::nullopt;
    }
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
    std::cerr << "execute_function: warning: the execute command does not take arguments "
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
    std::cerr << "execute_function: warning: whole input file is being "
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

auto prepend_file_name_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "prepend_file_name_function: the prepend_file_name command does not "
      "take arguments ignoring them" << std::endl;
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    if (context.file_stream.first) {
      (*context.operations_stream) = *context.file_stream.first
        + std::string(nl) + *context.operations_stream;
    } else {
      std::cerr << "prepend_file_name_function: no registered input file, did "
        "you forget to assign it in a test?" << std::endl;
    }
  }
  return context;
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

auto unamb_operations_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "unamb_operations_function: the unamb_operations command does not "
      "take arguments ignoring them" << std::endl;
  }
  if (!context.operations_stream) {
    return tl::make_unexpected("unamb_operations_function: operations_stream holds "
        "no value");
  }
  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream =
      (*context.operations_stream + std::string("$")
       + std::string(nl) + *context.operations_stream);
  }
  return context;
}

auto next_operation_space_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "next_operation_space_function: the next_operation_space command "
      "does not take arguments ignoring them" << std::endl;
  }
  size_t pos = 0;
  if (command.address && context.cycle == *command.address || !command.address) {
    if ((pos = context.file_stream.second.find(nl)) != std::string::npos) {
      context.result += (*context.operations_stream + std::string(nl));
      (*context.operations_stream)
        = std::string(context.file_stream.second.substr(0, pos));
      context.file_stream.second = context.file_stream.second.substr(pos + 1);
      // tricky, not mentioned in gnu sed manual
      context.cycle++;
    } else {
      context.current_command = context.commands.size();
    }
  }

  return context;
}

auto append_next_operation_space_function(Context context,
    const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "append_next_operation_space_function: the "
      "append_next_operation_space command does not take arguments ignoring them"
      << std::endl;
  }
  size_t pos = 0;
  if (command.address && context.cycle == *command.address || !command.address) {
    if ((pos = context.file_stream.second.find(nl)) != std::string::npos) {
      (*context.operations_stream)
        += (std::string(nl) + std::string(context.file_stream.second.substr(0, pos)));
      context.file_stream.second = context.file_stream.second.substr(pos + 1);
      // tricky, not mentioned in gnu sed manual
      context.cycle++;
    } else {
      context.current_command = context.commands.size();
    }
  }

  return context;
}

auto print_operations_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "print_operations_function: the print_operations command does not "
      "take arguments ignoring them" << std::endl;
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    context.result += *context.operations_stream
      + std::string(nl);
  }
  return context;
}

auto nl_print_operations_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "nl_print_operations_function: the nl_print_operations command does not "
      "take arguments ignoring them" << std::endl;
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    size_t pos;
    if ((pos = context.file_stream.second.find(nl)) != std::string::npos) {
      context.operations_stream = *context.operations_stream
        + std::string(nl) + context.operations_stream->substr(0, pos);
    } else {
      context.operations_stream = *context.operations_stream
        + std::string(nl) + *context.operations_stream;
    }
  }
  return context;
}

auto quit_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    std::cerr << "quit_function: no arguments provided, exiting with code 1"
      << std::endl;
  } else if (command.arguments->size() != 1) {
    std::cerr << "quit_function: quit expects 1 argument, exiting with code 1"
      << std::endl;
  }
  std::exit(std::stoi((*command.arguments)[0]));
}

auto read_in_file_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("read_in_file_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("read_in_file_function: read_in_file expects 1 argument");
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    if (context.stream_map.contains((*command.arguments)[0])) {
      if (!context.stream_map[(*command.arguments)[0]].is_open()) {
        return tl::make_unexpected("read_in_file_function: file exists in "
            "stream_map but is not open");
      }

      // reset it in case the file has already been read
      context.stream_map[(*command.arguments)[0]].clear();
      context.stream_map[(*command.arguments)[0]].seekg(0);
    } else {
      context.stream_map[(*command.arguments)[0]]
        = std::fstream((*command.arguments)[0]);
    }

    std::ostringstream buffer;
    buffer << context.stream_map[(*command.arguments)[0]].rdbuf();

    // if we don't do this there will be an extraneous nl for the majority of
    // files processed :(
    auto buffer_str = buffer.str();
    const auto nl_str = std::string(nl);
    if (buffer_str.size() >= nl_str.size()
        && buffer_str.compare(buffer_str.size() - nl_str.size(), nl_str.size(), nl_str) == 0) {
      buffer_str.erase(buffer_str.size() - nl_str.size());
    }

    (*context.operations_stream) += (std::string(nl) + buffer_str);
  }

  return context;
}

auto read_in_file_line_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("read_in_file_line_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("read_in_file_line_function: read_in_file_line "
        "expects 1 argument");
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    if (!context.stream_map.contains((*command.arguments)[0])) {
      context.stream_map[(*command.arguments)[0]] = std::fstream((*command.arguments)[0]);
    }
    if (!context.stream_map[(*command.arguments)[0]].is_open()) {
      return tl::make_unexpected("read_in_file_function: file exists in "
          "stream_map but is not open");
    }

    std::string line;
    if (std::getline(context.stream_map[(*command.arguments)[0]], line)) {
      (*context.operations_stream) += (std::string(nl) + line);
    }
  }
  return context;
}

auto substitute_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("substitute_function: no arguments provided");
  } else if (command.arguments->size() != 2) {
    return tl::make_unexpected("substitute_function: substitute expects 2 argument");
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    auto tmp = (*context.operations_stream);
    (*context.operations_stream) = std::regex_replace(*context.operations_stream,
        std::regex((*command.arguments)[0]), (*command.arguments)[1]);
    if (tmp != (*context.operations_stream)) {
      context.last_replace_success = true;
    } else {
      context.last_replace_success = false;
    }
  }

  return context;
}

auto find_label_index(const Context& context,
    const std::string label) -> std::optional<uint64_t> {
  for (uint64_t i = 0; i < context.commands.size(); i++) {
    const auto& command = context.commands[i];
    if (command.arguments
        && command.arguments->size() == 1
        && (command.name == ":" || command.name == "label")
        && (*command.arguments)[0] == label) {
      return i;
    }
  }
  return std::nullopt;
}

auto branch_true_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("branch_true_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("branch_true_function: branch_true expects 1 argument");
  }

  auto maybe_label = find_label_index(context, (*command.arguments)[0]);
  if (!maybe_label) {
    return tl::make_unexpected(std::string("branch_true_function: unable to "
          "find label ") + (*command.arguments)[0]);
  }
  if (context.last_replace_success) {
    context.current_command = *maybe_label;
  } else {
    context.current_command = context.commands.size();
  }

  return context;
}

auto assert_version_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("assert_version_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("assert_version_function: required_version expects "
        "1 argument");
  }

  if ((*command.arguments)[0] != sedim_version) {
    return tl::make_unexpected(
        std::string("assert_version_function: version required: ")
        + (*command.arguments)[0]
        + std::string(" does not match current version: ")
        + std::string(sedim_version));
  }
  return context;
}

auto append_to_file_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("append_to_file_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("append_to_file_function: append_to_file expects "
        "1 argument");
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    std::ofstream file_to_append;
    file_to_append.open((*command.arguments)[0], std::ios::app);
    if (!file_to_append) {
      return tl::make_unexpected(std::string("append_to_file_function: unable to "
            "open file with name: ") + (*command.arguments)[0]);
    }
    file_to_append << (*context.operations_stream) << nl;
  }
  return context;
}

auto nl_append_to_file_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("nl_append_to_file_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("nl_append_to_file_function: nl_append_to_file "
        "expects 1 argument");
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    std::ofstream file_to_append;
    file_to_append.open((*command.arguments)[0], std::ios::app);
    if (!file_to_append) {
      return tl::make_unexpected(std::string("nl_append_to_file_function: unable to "
            "open file with name: ") + (*command.arguments)[0]);
    }
    size_t pos = (*context.operations_stream).find((*command.arguments)[0]);
    if (pos != std::string::npos) {
      file_to_append << (*context.operations_stream).substr(0, pos + 1);
    } else {
      file_to_append << (*context.operations_stream) << nl;
    }
  }
  return context;
}

auto exchange_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "exchange_function: the exchange command does not take "
      "arguments ignoring them" << std::endl;
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    auto tmp = *context.operations_stream;
    context.operations_stream = context.static_stream;
    context.static_stream = tmp;
  }
  return context;
}

auto translate_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("translate_function: no arguments provided");
  } else if (command.arguments->size() != 2) {
    return tl::make_unexpected("translate_function: translate expects 2 arguments");
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

auto zap_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "zap_function: the zap command does not take arguments "
      "ignoring them" << std::endl;
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream = "";
  }
  return context;
}

auto prepend_line_no_function(Context context, const Command& command) -> ResultContext {
  if (command.arguments) {
    std::cerr << "prepend_line_no_function: the prepend_line_no command does not "
      "take arguments ignoring them" << std::endl;
  }

  if (command.address && context.cycle == *command.address || !command.address) {
    context.operations_stream = std::to_string(context.cycle)
      + std::string(nl) + *context.operations_stream;
  }
  return context;
}

auto verify_label_function(Context context, const Command& command) -> ResultContext {
  if (!command.arguments) {
    return tl::make_unexpected("verify_label_function: no arguments provided");
  } else if (command.arguments->size() != 1) {
    return tl::make_unexpected("verify_label_function: label expects 1 argument");
  }
  if (command.address) {
    return tl::make_unexpected("verify_label_function: label expects no address");
  }
  return context;
}

using SemanticFunc = std::function<ResultContext(Context, const Command&)>;
using CommandSemanticUMap = std::unordered_map<std::string, SemanticFunc>;
static inline auto control_flow_map = CommandSemanticUMap {
  {"a",                           append_function},
  {"append",                      append_function},
  {"c",                           change_function},
  {"change",                      change_function},
  {"d",                           delete_function},
  {"delete",                      delete_function},
  {"D",                           delete_restart_function},
  {"delete_restart",              delete_restart_function},
  {"i",                           insert_function},
  {"insert",                      insert_function},
  {"e",                           execute_function},
  {"execute",                     execute_function},
  {"F",                           prepend_file_name_function},
  {"prepend_file_name",           prepend_file_name_function},
  {"h",                           add_to_static_function},
  {"add_to_static",               add_to_static_function},
  {"H",                           nl_add_to_static_function},
  {"nl_add_to_static",            nl_add_to_static_function},
  {"g",                           replace_operation_function},
  {"replace_operation",           replace_operation_function},
  {"G",                           nl_replace_operation_function},
  {"nl_replace_operation",        nl_replace_operation_function},
  {"l",                           unamb_operations_function},
  {"unamb_operations_stream",     unamb_operations_function},
  {"n",                           next_operation_space_function},
  {"next_operation_space",        next_operation_space_function},
  {"N",                           append_next_operation_space_function},
  {"append_next_operation_space", append_next_operation_space_function},
  {"p",                           print_operations_function},
  {"print",                       print_operations_function},
  {"P",                           nl_print_operations_function},
  {"nl_print",                    nl_print_operations_function},
  {"q",                           quit_function},
  {"Q",                           quit_function},
  {"quit",                        quit_function},
  {"r",                           read_in_file_function},
  {"read_in_file",                read_in_file_function},
  {"R",                           read_in_file_line_function},
  {"read_in_file_line",           read_in_file_line_function},
  {"s",                           substitute_function},
  {"substitute",                  substitute_function},
  {"t",                           branch_true_function},
  {"branch_true",                 branch_true_function},
  {"v",                           assert_version_function},
  {"required_version",            assert_version_function},
  {"w",                           append_to_file_function},
  {"append_to_file",              append_to_file_function},
  {"W",                           nl_append_to_file_function},
  {"nl_append_to_file",           nl_append_to_file_function},
  {"x",                           exchange_function},
  {"exchange",                    exchange_function},
  {"y",                           translate_function},
  {"translate",                   translate_function},
  {"z",                           zap_function},
  {"zap",                         zap_function},
  {"=",                           prepend_line_no_function},
  {"prepend_line_no",             prepend_line_no_function},
  {":",                           verify_label_function},
  {"label",                       verify_label_function},
};

auto execute_from_files(const std::string& input_file,
    const std::string& command_file) -> std::string {
  auto maybe_input = file_to_string(input_file);
  if (!maybe_input) {
    throw std::runtime_error(maybe_input.error());
  }
  auto input_text = maybe_input.value();

  auto maybe_json = file_to_string(command_file);
  if (!maybe_json) {
    throw std::runtime_error(maybe_json.error());
  }
  auto command_text = maybe_json.value();
  return execute(input_text, command_text, input_file);
}

auto execute(const std::string& input_text, const std::string& command_text,
    const std::optional<std::string>& file_name,
    const TextToCommands& text_to_commands) -> std::string {

  auto context = Context(std::make_pair(file_name, input_text));
  auto maybe_commands = text_to_commands(command_text);
  if (!maybe_commands) {
    throw std::runtime_error(std::string("execute: unable to parse json: ")
        + maybe_commands.error());
  }
  context.commands = maybe_commands.value();

  size_t pos = 0;
  while ((pos = context.file_stream.second.find(nl)) != std::string::npos) {
    context.operations_stream = context.file_stream.second.substr(0, pos);
    context.file_stream.second = context.file_stream.second.substr(pos + 1);
    context.cycle++;
    context.last_replace_success = false;
    context.current_command = 0;
    while (context.current_command < context.commands.size()) {
      const auto& command = context.commands[context.current_command];
      if (control_flow_map.contains(command.name)) {
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
      context.current_command++;
    }
    if (context.operations_stream) {
      context.result += (*context.operations_stream + std::string(nl));
    }
  }
  return context.result;
}
