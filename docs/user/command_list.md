# :microscope: Current `sim` Command List
  - append or a: This `Command` appends a newline then the argument of the
    command to the current `operation_stream`. This functionality is fully
    supported comparative to the GNU `sed` program.
  - branch or b: This `Command` branches unconditionally to a label. If the
    label is not specified then it will end the script for the current
    `operation_stream`. This functionality is fully supported comparative to
    the GNU `sed` program.
  - change or c: This `Command` will change the `operation_stream` to its
    argument. This functionality is fully supported comparative to the GNU
    `sed` program.
  - delete or d: This `Command` will delete the current `operation_stream` then
    it will end the script for the current `operation_stream`. This
    functionality is fully supported comparative to the GNU `sed` program.
  - delete_restart or D: This `Command` will remove the prefix of the
    `operation_stream` up to the first newline. If there is no newline in the
    `operation_stream` then it will end the script for the current
    `operation_stream`. This functionality is fully supported comparative to the
    GNU `sed` program.
  - execute or e: This `Command` will execute literally what is in the
    `operation_stream`.  This functionality is not fully supported comparative
    to the GNU `sed` program as it does not yet accept arguments which would be
    a command to execute. See #2.
  - prepend_file_name or F: This `Command` will prepend the name of the input
    file then a newline to the current `operation_stream`. This functionality is
    fully supported comparative to the GNU `sed` program.
  - insert or i: This `Command` will insert its argument then a newline before
    the current `operation_stream`. This functionality is
    fully supported comparative to the GNU `sed` program.
  - replace_operation or g: This `Command` will replace the current
    `operation_stream` with the current `static_stream`. This functionality is
    fully supported comparative to the GNU `sed` program.
  - nl_replace_operation or G: This `Command` will append a newline then, if it
    exists, the current `static_stream` to the `operation_stream`. This
    functionality is fully supported comparative to the GNU `sed` program.
  - add_to_static or h: This `Command` will replace the current `static_stream`
    with the current `operation_stream`. This functionality is fully supported
    comparative to the GNU `sed` program.
  - nl_add_to_static or H: This `Command` will add to the current
    `static_stream` a newline then the contents of the `operation_stream`. This
    functionality is fully supported comparative to the GNU `sed` program.
  - unamb_operations_stream or l: This `Command` will unambiguously print the
    current `operation_stream`. This function is not fully supported comparative
    to the GNU `sed` program. The reason for this being is that it does not
    currently print explicit whitespace (i.e. GNU `sed` would print a newline as
    "\n" but `sim` does not yet). See #4.
  - next_operation_space or n: If there is a newline after the current
    `operation_stream` in the `file_stream` file contents then this `Command`
    will put the current `operation_stream` into the `result` (to be printed)
    then it will read the next line into the `operation_stream`, otherwise it
    will end the script. This `Command` is not fully supported comparative to the
    GNU `sed` program. The reason for this being is that that there is no
    concept of "auto-print". See #5.
  - append_next_operation_space or N: If there is a newline after the current
    `operation_stream` in the `file_stream` file contents then this `Command`
    will append a newline then append up to the next newline from the
    `file_stream` file contents onto the `operation_stream` otherwise it will
    end the script for the current `operation_stream`. This functionality is
    fully supported comparative to the GNU `sed` program.
  - print or p: This `Command` will add the current `operation_stream` to the
    `result` to be printed. This functionality is fully supported comparative to
    the GNU `sed` program.
  - nl_print or P: If there is a newline after the current `operation_stream`
    in the `file_stream` file contents then this `Command` will append a newline
    and the next line of the `file_stream` file contents to the current
    `operation_stream` otherwise it will append a newline then the current
    `operation_stream` to the `operation_stream`. This functionality is fully
    supported comparative to the GNU `sed` program.
  - quit or q: This `Command` will quit the program with its argument as the
    exit code. This `Command` is not fully supported comparative to the GNU
    `sed` program due to the logistics of working in exiting and printing the
    current contents of `result` not working well with my current
    infrastructure.  I am unsure if I will support this functionality going
    forward.
  - quit or Q: This `Command`  will quit the program with its argument as the
    exit code. This functionality is fully supported comparative to the GNU
    `sed` program.
  - read_in_file or r: This `Command` will append a newline to the current
    `operation_stream` then read the file with the name of its
    argument, then append its contents to the `operation_stream`. This
    functionality is fully supported comparative to the GNU `sed` program.
  - read_in_file_line or R: This `Command` will append a newline to the current
    `operation_stream` then if the file which is its argument is not open it
    will open it and append its next line to the current `operation_stream`,
    otherwise it will append the next line of the open file to the
    `operation_stream`. Note that the file is persistently open so you can read
    in new lines across commands. This functionality is fully supported
    comparative to the GNU `sed` program.
  - substitute or s: This `Command` will substitute the pattern of the first
    argument to the pattern of the second argument when applied to an
    `operation_stream`. Note if a replacement is made it sets
    `last_replace_success` to true. These arguments are fed directly into C++'s
    regex library calls. Remember to escape your backslashes. This functionality
    is not fully supported comparative to the GNU `sed` program as there are no
    accepted flags yet, see #6.
  - branch_true or t: This `Command` will branch to the label of its argument
    it `last_replace_success` success is true. If the label does not exist then
    it will end the script for the current `operation_stream`. This
    functionality is fully supported comparative to the GNU `sed` program.
  - branch_false or T: This `Command` will branch to the label of its argument
    it `last_replace_success` success is false. If the label does not exist then
    it will end the script for the current `operation_stream`. This
    functionality is fully supported comparative to the GNU `sed` program.
  - required_version or v: This `Command` will assert that the current version
    of `sim` is equal to its argument. This functionality is fully supported
    comparative to the GNU `sed` program.
  - append_to_file or w: This `Command` will append the `operation_stream` to
    the file with the name of its argument followed by a newline. This
    functionality is fully supported comparative to the GNU `sed` program.
  - nl_append_to_file or W: This `Command` will append up to the first newline
    in the `operation_stream` to the file with the name of its argument (if there
    is no newline in the `operation_stream` it will append the whole
    `operation_stream`). This functionality is fully supported comparative to
    the GNU `sed` program.
  - exchange or x: This `Command` will exchange the current `static_stream` and
    `operation_stream`. This functionality is fully supported comparative to the
    GNU `sed` program.
  - translate or y: This `Command` will translate all instances of its first
    argument in the `operation_stream` to its second argument.  This
    functionality is fully supported comparative to the GNU `sed` program.
  - zap or z: This `Command` will empty the contents of the `operation_stream`.
    This functionality is fully supported comparative to the GNU `sed` program.
  - prepend_line_no or =: This `Command` will prepend the current line number
    and a newline to the current `operation_stream`. This functionality is
    fully supported comparative to the GNU `sed` program.
  - label or ":": This `Command` will create and ensure the validity of a label
    (i.e. make sure it has one argument and make sure it does not have an
    address). This functionality is fully supported comparative to the GNU `sed`
    program.
  - #: This `Command` is meaningless in `sim`'s current front end as it is
    encoded in json, so it will not be supported. Feel free to write your own
    front end and use this implementation:
```
auto comment_function(Context context, const Command& command) -> ResultContext {
    // to surpress warnings
    (void)(command);
    return context;
}

{"#", comment_function},
```
