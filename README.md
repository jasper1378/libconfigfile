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

Config files can be read from a physical file or a provided input stream. To parse from a physical file, call `parse_file()`. This function takes a single `std::filesystem::path` argument representing the file path (should be absolute, not relative) To parse from a provided input stream, call `parse()`. This function takes a `std::string` argument identifying the stream, a `std::istream` reference argument corresponding to the stream to read from, and a defaul boolean argument specfying whether the identifier is a valid file path and the stream corresponds to a physical file (used for determining relative file paths for included sub-files). Both functions return a data structure (see below) representing the parsed file, and possibly throw exceptions during the process (see below).

### Data structures (`node` class hierarchy)

All syntactical constructs (with the exception of directives and comments) within the configuration file can be represented by a class derived from `node`. This class hierarchy is a mix of abstract and concrete classes. It is shown visually below, abstract classes are marked with `*`.

```
*node
├── section_node
└── *value_node
    ├── array_value_node
    └── *end_value_node
        ├── integer_end_value_node
        ├── float_end_value_node
        └── string_end_value_node
```

Abstract classes (`node`, `value_node`, `end_value_node`) correspond to increasingly specific classifications of config file constructs. Concrete classes (`section_node`, `array_value_node`, `integer_end_value_node`, `float_end_value_node`, `string_end_value_node`) correspond to the actual types of information found within a config file. Many of these concrete classes provide interfaces similar to familiar standard library classes (`section_node` : `std::unordered_map`, `array_value_node`: `std::vector`, `string_end_value_node`: `std::string`). Others provide a simple get/set interface (`integer_end_value_node`, `float_end_value_node`).

The hierarchy is designed in such a way as to promote polymorphic usage. The actual, pointed-to type of a polymorphic pointer can be identified by calling the increasingly more specific `get_node_type()`, `get_value_node_type()`, and `get_end_value_node_type()` member functions, which return an `enum` value corresponding to the appropriate abstract or concrete child class. Which of these member functions are available for a given `node` class depends on its location in the hierarchy. If only the concrete class that the pointer is ultimately pointing to is of interest, `get_absolute_node_type()` can be used. Details are given below.


- `get_node_type()`
    - returns:
        - `node_type::VALUE`
        - `node_type::SECTION`
    - member of:
        - `node` and children
- `get_value_node_type()`
    - returns:
        - `value_node_type::END_VALUE`
        - `value_node_type::ARRAY`
    - member of:
        - `value_node` and children
- `get_end_value_node_type()`
    - returns:
        - `end_value_node_type::INTEGER`
        - `end_value_node_type::FLOAT`
        - `end_value_node_type::STRING`
    - member of:
        - `end_value_node` and children
- `get_absolute_node_type()`
    - returns:
        - `absolute_node_type::SECTION`
        - `absolute_node_type::ARRAY`
        - `absolute_node_type::INTEGER`
        - `absolute_node_type::FLOAT`
        - `absolute_node_type::STRING`
    - member of
        - all `node` classes

To avoid the hassle of dealing with a bare polymorphic `node` (or child) pointer (memory leaks, checking success of `dynamic_cast`, etc,), the smart pointer class `node_ptr` can be used. In order to maintain a degree of harmony with the library interfaces, `node`-derived classes should always be used and managed through a `node_ptr`. This class is similar to `std::unique_ptr` in that it is responsible for deallocating any resources associated with the pointer when it goes out of scope. However, its specialized nature (will always be used with a `node`-derived class, usually polymorphically) means that it can offer additional featues. `node_ptr` is designed in such a way that the pointer component is completely abstracted and the object obtains value semantics. `node_ptr` is a templated class taking two parameters. The first is a type parameter that specificies which `node`-derived the `node_ptr` is pointing to; this is enforced using concepts. The second is a boolean parameter specifying whether two `node_ptr`s should be compared by address or by pointed-to value, this defaults to comparing by address as that is the behaviour of `std::unique_ptr`. `node_ptr` supports all of the same options as `std::unique_ptr`. `node_ptr`s are both movable (transfers ownership of pointed-to resource) and copyable (copies pointed-to resource). `node_ptr`s can be easily constructed by calling the non-member function `make_node_ptr()` which behaves similarly to `std::make_unique`. This function requires the same template arguments as `node_ptr` and forwards its arguments to the constructor of the pointed-to resource. Two types of `node_ptr` are implicitly convertible to one another if: the type of the pointed-to `node` class of the "to" `node_ptr` is a base of the type of pointed-to `node` class of the "from" `node_ptr`; or, they point to the same type of `node` class and differ only in whether they are compared by address or value. One type of `node_ptr` can be explictly cast to another by calling the non-member function `node_ptr_cast`, which behaves similarly to a checked `dynamic_cast` between pointed-to resources. There exist variants of `node_ptr_cast` supporting both copy and move semantics. This function will throw if the cast is not possible. To avoid this, you can check whether the cast is possible by calling the non-member function `node_ptr_is_castable`. There exists a host of functions for explicitly comparing two `node_ptr`s by address or value regardless of the method specified by their template argument. Printing a `node_ptr` will print the pointed-to value rather than the address.

### Error handling

While calling `libconfigfile::parser()`, errors resulting in the parser itself (such as being unable to open a file) with be thrown as `std::runtime_error`. If the parser detects a violation of the syntax specification (see above) a `libconfigfile::syntax_error` will be thrown. This class is derived from `std::runtime_error` and behaves similarily. Its `what_arg` will be a string containing the line and char position of the error, as well as a brief description of what went wrong. This string is suitable for displaying to the end user.

## Future improvement

The following are a few of the improvements that I would like to implement in libconfigfile sometime in the future.

- Improve standardization and phrasing of error messages
- Provide a "snapshot" into file at the location of a syntax error (see GCC for an example)
- Functionality to verify semantic correctness of config file (would rely on another file provided by the application developer specifying which options should be set and what data types they should be)
- Unicode support for strings

## License
See [LICENSE.md](LICENSE.md) for details.
