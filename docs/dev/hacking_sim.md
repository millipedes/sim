# :microscope: Hacking sim
There are three primary ways to hack `sim` which will be covered here:
  1. Add custom `Command`s.
  2. Add a new front end (the way in which `Command`s are encoded).
  3. Change the delimiter i.e. if you want to split the commands on a character
     other than a newline.

## Add Custom `Command`s
Fundamentally the way in which commands are processed is with the following
structure:
```
// In English
A map from a string, the Command's name to a function which takes a Context and
the Command isntance to be processed and returns the modified Context or an
Error in the form of a string if an error occured (Context is returned because
it is moved around to follow a functional style).

// In C++
using ResultContext = tl::expected<Context, std::string>;
using SemanticFunc = std::function<ResultContext(Context, const Command&)>;
using CommandSemanticUMap = std::unordered_map<std::string, SemanticFunc>;
static inline auto control_flow_map = CommandSemanticUMap {
    // Command names and their semantic function.
};
```

So all you have to do is make a new name for your `Command` let's say
`my_new_command` and a function for its semantic meaning let's call it
`my_new_command_function`. What does this look like:

```
auto my_new_command_function(Context context, const Command& command) -> ResultContext {
    if (error) {
        return tl::make_unexpected("Why did this function fail?");
    }
    // modify the context
    return context;
}

...


static inline auto control_flow_map = CommandSemanticUMap {
    ...
    {"my_new_command", my_new_command_function},
    ...
};

```

And just like that your command is now in `sim`! Additionally, note that the
commands are all self-documenting this way. If you wonder what one does it will
be fully contained in its corresponding function.

Okay, but what's the big deal, what semantic actions can I take? Well `sim` is
actually turing complete, so you have quite a bit to work with in terms of what
you have to work with. To start with you have all existing `Command`s, but
beyond that you can access the following information using the `Context` data
structure:
  1. `file_stream`
    - The current input file's name
    - The current file's contents (at least it's head, as you read from the file
      its contents will be discarded).
  2. `stream_map`
    - Map of a file name.
    - To its contents.
  3. `operations_stream`
    - The current line which is being processed.
  4. `static_stream`
    - A stream which will no change during the control flow of the program (i.e.
      the user can modify it). This is the persistent storage in what makes it
      turing complete.
  5. `commands`
    - The list of commands which the user has input in order.
  6. `result`
    - What will be printed at the end of the program.
  7. `cycle`
    - Synonymous with the line number.
  8. `current_command`
    - This is what number in the order of commands will be executed. Upon
      completion of the input script for each line to be processed this will be
      incremented. This allows for recursion in the turing completeness, see the
      implementation of `branch`, `branch_true`, and `branch_false`.
  9. `last_replace_success`
     - This is indicative of if anything was replaced in the last `substitute`
       command (used by `branch_true` and `branch_false`).

Using just the `Command`s there is a terminal tetris implementation floating
around on git, so the sky is the limit with adding custom commands to improve
your workflows!

## Add a New Front End
I don't mind json, but it is not for everyone.  If you want a different input
encoding for commands, you can change this quite easily! The primary control
flow for `sim` is via two functions:
```
using ResultCommands = tl::expected<Commands, std::string>;
using TextToCommands = std::function<ResultCommands(const std::string&)>;

// This function is what the main loop calls
auto execute_from_files(const std::string& input_file,
    const std::string& command_file) -> std::string;

// This function is called by execute_from_files with the input file name, but
// not the text_to_commands.
auto execute(const std::string& input_text, const std::string& command_text,
    const std::optional<std::string>& file_name = std::nullopt,
    const TextToCommands& text_to_commands = parse_json) -> std::string;
```

Since `execute` relies on a function which takes in a `const std::string&` and
returns a `ResultCommands`, if you want to make a function which fits this
signature you can specify it in `execute_from_files` when it calls `execute`.
For example:

```
auto parse_my_favorite_input_format(const std::string& input) -> ResultCommands {
    // parse input how you desire to do so
}

auto execute_from_files(const std::string& input_file,
    const std::string& command_file) -> std::string {
  return execute(input_text, command_text, input_file,
      // add an explicit arg here to use your input format
      parse_my_favorite_input_format);
}
```

## Changing the Delimiter
This is relatively simple, the `nl` string is used as the delimiter, hence to
change your delimiter you can edit what is in these lines:
```
#if defined(_WIN32) || defined(_WIN64)
  static constexpr auto nl = "\r\n";
#else
  static constexpr auto nl = "\n";
#endif
```
