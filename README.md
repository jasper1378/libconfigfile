# libconfigfile

## About
libconfigfile is a C++ configuration file parsing library. 

## Note
This project is currently in progress. Do not use!

## Installation
Acquire the sources
```
$ git clone https://github.com/jasper1378/libconfigfile.git
$ cd libconfigfile
```
Build
```
$ make
```
Install
```
$ make install
```

## Usage
The Makefile will install both static and shared versions of the library, use whichever you prefer. Most users will just want to include `libconfigfile.hpp`, which itself includes all the other headers. All code in the library is found within the `libconfigfile` namespace.

### Configuration file syntax specification

For a description of the syntax used in the configuration files parsed by this library, see [configuration_file_syntax_specification.md](configuration_file_syntax_specification.md).

### Parsing a file

The main function of this library, parsing a configuration file, can be accomplished by calling `libconfigfile::parse()`. This function takes a single `std::filesystem::path` argument representing the file path (should be absolute, not relative) and returns a data structure (see below) representing the parsed file, possibly throwing exceptions during the process (see below).

### Data structures (`node` class hierarchy)

```
node
├── section_node
└── value_node
    ├── array_value_node
    └── end_value_node
        ├── integer_end_value_node
        ├── float_end_value_node
        └── string_end_value_node
```

### Error handling

While calling `libconfigfile::parser()`, errors resulting in the parser itself (such as being unable to open a file) with be thrown as `std::runtime_error`. If the parser detects a violation of the syntax specification (see above) a `libconfigfile::syntax_error` will be thrown. This class is derived from `std::runtime_error` and behaves similarily. Its `what_arg` will be a string containing the line and char position of the error, as well as a brief description of what went wrong. This string is suitable for displaying to the end user.

## License
See [LICENSE.md](LICENSE.md) for details.
