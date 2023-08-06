# Configuration File Syntax Specification

Version: 3

## Contents
- [Introduction](#introduction)
- [Text](#text)
- [Values](#values)
- [Strings](#strings)
- [Integers](#integers)
- [Floats](#floats)
- [Arrays](#arrays)
- [Maps](#maps)
- [Comments](#comments)
- [Directives](#directives)
- [Filename extension](#filename-extension)

## Introduction

The libconfigfile syntax specification is a text format for the storage and retrieval of structured data in configuration files. The syntax, as well as the wording of this document, is inspired by [JSON](https://www.json.org/json-en.html), [TOML](https://toml.io/en/), and [libconfig](https://hyperrealm.github.io/libconfig/).

Libconfigfile can represent three primitive types (strings, integer, and floats) and two structured types (arrays and maps).

A string is a sequence of zero or more ASCII characters.

An array is an ordered sequence of zero or more values.

A map is an unordered collection of zero or more name/value pairs.

Libconfigfile also supports comments and directives, which do not follow the usual rules of syntax.

## Text

A libconfigfile text is a sequence of tokens formed from ASCII characters that conform to the libconfigfile value grammar. The set of tokens includes seven structural tokens, strings, integer and floats.

The seven structural tokens:
- '[' 0x5b left square bracket
- '{' 0x7b left curly bracket
- ']' 0x5d right square bracket
- '}' 0x7d right curly bracket
- '=' 0x3d equals sign
- ',' 0x2c comma
- ';' 0x3b semicolon

Insignificant whitespace is allowed before or after any token. Whitespace is any sequence of one or more of the following characters: character tabulation (0x09), line feed (0x0a), and space (0x20). Whitespace is not allowed within any token, except that space is allowed in strings.

## Values

A value can be a string, integer, float, array, or map.

## Strings

A string is a sequence of ASCII characters wrapped with quotation marks (0x22). All characters may be placed within the quotation marks except for the characters that must be escaped: quotation mark (0x22), reverse solidus (0x5c), and the control characters 0x00 to 0x1f. There are two-character escape sequence representations of some characters.
- '\"' represents the quotation mark character (0x22).
- '\\' represents the reverse solidus character (0x5c).
- '\/' represents the solidus character (0x2f).
- '\b' represents the backspace character (0x08).
- '\f' represents the form feed character (0x0c).
- '\n' represents the line feed character (0x0a).
- '\r' represents the carriage return character (0x0d).
- '\t' represents the character tabulation character (0x09).

So, for example, a string containing only a single reverse solidus character may be represented as "\\".

Any character may be represented as a hexadecimal escape sequence. It is represented as a four-character sequence: a reverse solidus, followed by the lowercase letter 'x', followed by two hexadecimal the encode the character. Hexedecimal digits can be digits (0x30 through 0x39) or the hexadecimal letters 'A' through 'F' in uppercase (0x41 through 0x46) or lowercase (0x61 through 0x66). So, for example, a string containing only a single reverse solidus character may be represented as "\x5c".

The following four cases all produce the same result:
- "\x2f"
- "\x2F"
- "\/"
- "/"

Adjacent strings separated only by whitespace will be concatenated.

## Integers

An integer is a sequence of decimal, binary, octal, or hexadecimal digits. It may have a preceeding minus sign (0x2d) or plus sign (0x2b). Decimal integers are not prefixed; the digits are the characters 0x30 through 0x39. Binary integers must be prefixed by '0b' or '0B'; the digits are the characters 0x30 through 0x31. Octal integers must be prefixed by '0o' or '0O'; the digits are the characters 0x30 through 0x37. Hexadecimal integers must be prefixed by '0x' or '0X'; the digts are the characters 0x30 through 0x39, 0x61 through 0x66, and 0x41 through 46. The digits may be separated by an underscore (0x5f). Integers are stored as signed 64-bit integers.

## Floats

A float is a sequence of decimal digits. It may have a preceding minus sign (0x2d) or plus sign (0x2b). It may have a fractional part prefixed by a decimal point (0x2e). It may have an exponent, prefixed by 'e' (0x65) or 'E' (0x45) and optionally '+' (0x2b) or - (0x2d). The digits are the characters 0x30 through 0x39. The digits may be separated by an underscore (0x5f). The digits may be replaced with the special values: infinity (represented by 'inf') or not-a-number ('nan'); these are case-insensitive. Floats are stored as IEEE 754 binary 64 values.

## Arrays

An array structure is a pair of square bracket tokens surrounding zero or more values. The value are separated by commas. The ordering of values is preserved.

## Maps

A map structure is represented by a pair of curly bracket tokens surrounding zero or more name/value pairs. A name is a sequence of one or more of the following characters: letters (0x41-0x5a, 0x61-0x7a), digits (0x30-0x39), underscores (0x5f), and dashes (0x2d). Names must be unique in their scope. A single equals sign follows each name, separating the name from the value. A single semicolon terminates a value. The ordering of name/value pairs is not preserved. The root of a configuration file consists of an unamed map.

## Comments

There are three styles of comments are allowed in configuration files. Script-style comments: all text beginning with a number sign (0x23) to the end of the line is ignored. C-style comments: all text, including line breaks, between a starting solidus-asterisk sequence (0x2f, 0x2a) to an ending asterisk-solidus sequence is ignored. C++-style comments: All text beginning with a solidus-solidus sequence (0x2f, 0x2f) to the end of the line is ignored. Comments are allowed wherever whitespace is permitted.

## Directives

Directives provide a means for special commands to be passed to the parser. Directives consist of an at sign (0x40) followed by the name of the directive, and optionally, argumuents. Directives are only allowed in the root map. The directive leader, name, and arguments must appear on a single line containing no other text. If arguments are present at least one whitespace character is required between the end of the name and the start of the arguments. Arguments are formatted and delimited according to the rules of the specific directive. There currently exists two directives.

The 'version' directive provides a means to ensure that the parser and a configuration file are using compatible versions of the syntax specification. This directive requires one argument: a version number corresponding to the syntax specification being used, enclosed by double quotes (0x22). At the top of this document can be found the version of the syntax specification that it is describing. If the parser encounters a syntax specification version that it is unable to parse, an error will be thrown. If there exists multiple version directives in a given document, their values should be identical.

the 'include' directives provides a means to include the contents of another file into the current configuratio file. This directive requires one argument: the path of the file to be included, enclosed by double quotes (0x22). The same escape sequences that are allowed in string values are allowed in the file path argument; however, adjacent strings will not be concatenated. Any included files must be indenpendently syntactically valid. Cyclical includes are, of course, impossible to parse.

## Filename extension

Configuration files using this syntax specification require no particular filename extension, however, '.conf' is suggested.
