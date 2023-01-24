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

Proper documentation will be coming the future. If you have a question in the meantime either: a) open an issue on GitHub (I'll try to respond as soon as a I can) or b) just look at the code (it's fairly straightforward and simple).

If the library is lacking functionality that you would find useful, feel free to open an issue on GitHub, there's a good chance that I'll implement it.

## Config File Syntax Specification
See [configuration_file_syntax_specification.md](configuration_file_syntax_specification.md) for details.

## License
See [LICENSE.md](LICENSE.md) for details.
