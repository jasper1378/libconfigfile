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

A map structure is represented by a pair of curly bracket tokens surrounding zero or more name/value pairs. A name is a sequence of one or more of the following characters: letters (0x41-0x5a, 0x61-0x7a), digits (0x30-0x39), underscores (0x5f), and dashes (0x2d). Names must be unique in their scope. A single equals sign follows each name, separating the name from the value. A single semicolon terminates a value. The ordering of name/value pairs is not preserved.

## Comments

## Directives

////

## Contents
- [Spec](#spec)
- [Comments](#comments)
- [Key-Value Pairs](#key-value-pairs)
- [Strings](#strings)
- [Integers](#integers)
- [Floats](#floats)
- [Booleans](#booleans)
- [Arrays](#arrays)
- [Maps](#maps)
- [Directives](#directives)
    - [Version Directive](#version-directive)
    - [Include Directive](#include-directive)
- [Filename Extension](#filename-extension)
- [Thanks](#thanks)

## Spec
- Config files are case-sensitive.
- libconfigfile does not support Unicode. All config files must contain only 8-bit ASCII text.
- Whitespace means tab (0x09) or space (0x20).
- Newline means LF (0x0A).

## Comments
- There are three types of comments allowed in a config file.
    - Script-style comments. All text beginning with a `#` character to the end of the line is ignored.
    - C-style comments. All text, including line breaks, between a starting `/*` sequence and an ending `*/` sequence is ignored.
    - C++-style comments. All text beginning with a `//` sequence to the end of the line is ignored.
- Comment delimiters appearing within quoted strings are treated as literal
  text.
```
# full-line script-style comment

key1 = "value"; # end-of-line script-style comment
key2 /* mid-line C-style comment */ = "value";
key3 = "value"; // end-of-line C++-style comment

key4 = "/* string, not a comment */";
```

## Key-Value Pairs
- The primary building block of a config file is the key-value pair.
- Keys are on the left of the equal signand values are on the right.
  Whitespace is ignored around key names and values.
- A key-value pair must be terminated by a semicolon.
- Keys may only containt ACII letters, ASCII digits, and underscores. Note that
  keys composed only of ASCII digits (eg. `1234`) are allowed, but are always
  interpreted as strings.
- Key names must appear completely on one line.
- Undefined keys are invalid.
- Defining a key multiple times in the same scope is invalid.
- Undefined values are invalid.
- Values may have one of the following types.
    - String
    - Integer
    - Float
    - Array
    - Map
```
key1 = "value";
key2 = ; // invalid
key3 = "value" // missing semicolon
= "value"; // invalid

key_4 = "value";
1234 = "value";

name = "Jasper";
name = "Young"; // invalid

map1 = {
    name = "jasper1378"; // valid
};
```

## Strings
- String values consist of arbitrary text delimited by double quotes.
- Adjacent strings are automatically concatenated, as in C/C++ source code. This
  is useful for formatting very long strings as sequences of shorter sequences.
- A backslash within a string marks an escape sequence. The following escape
  sequences are supported.

Escape Sequence | Hex value in ASCII | Character represented
---|---|---
`\a` | alert | `0x07`
`\b` | backspace | `0x08`
`\f` | formfeed page break | `0x0C`
`\n` | newline | `0x0A`
`\r` | carriage return | `0x0D`
`\t` | horizontal tab | `0x09`
`\v` | vertical tab | `0x0B`
`\\` | backslash | `0x5C`
`\'` | single quotation mark | `0x27`
`\"` | double quotation mark | `0x22`
`\xhh` | 8-bit ASCII value<br>represented by the<br>hexadecimal number `hh`

```
value1 = "I'm a string. \"You can quote me\".";

// the below are all equivalent

value1 = "the quick brown fox jumps over the lazy dog,";

value2 = "The quick brown fox"
         " jumps over the lazy dog"; // indentation does not matter

value3 = "The quick" /* comment */ " brown fox " // another comment
         "jumps over the lazy dog.";
```

## Integers
- Integers are whole numbers. Positive numbers may be prefixed with a plus sign. Negative numbers must be prefixed with a negative sign.
- For large numbers, you may use underscores between digits to enhance readability. Each underscore must be surrounded by at least one digit on each side.
- Integers values must be within the range of a signed 64-bit integer ((-2^(63)) to (2^(63)-1)).
- `-0` and `+0` are valid and identical to an unprefixed zero.
- Integer values may also be expressed in hexadecimal (`0x` or `0X` prefix), octal (`0o` or `0O` prefix), or binary (`0b` or `0B` prefix) formats. Hexadecimal values are case-insensitive.
```
int1 = +99;
int2 = 42;
int3 = 0;
int4 = -17;
int5 = 1_000;
int6 = 5_349_221;
int7 = _100_000; // invalid
int8 = 10_223_372_036_854_775_807; // too large

hex1 = 0xBADC0DE;
hex2 = 0xbadc0de;
hex3 = 0xbad_c0de;

oct1 = 0o1234567;
oct2 = 0o755;

bin1 = 0b11010110;
bin2 = -0b11010110;
```

## Floats
- Floats are implemented as IEEE 754 binary64 values.
- A float consists of an integer part followed by a fractional part and/or an exponent part. If both a fractional part and exponent part are present, the fractional part must precede the exponent part.
- A fractional part is a decimal point followed by one or more digits.
- An exponent part is an `E` (case-insensitive) followed by an integer part.
- The decimal point, if used, must surrounded by at least one digit one each side.
- Similar to integers, you may use underscores to enhance readability. Each underscore must surrounded by at least one digit.
- `-0.0` and `+0.0` are valid and map according to IEEE 754.
- Special float values include positive/negative infinity (`inf`) and positive/negative not-a-number (`nan`), they are case-insensitive.
```
flt1 = +1.0;
flt2 = 3.1415;
flt3 = -0.01;

flt4 = 5e+22;
flt5 = 1e06;
flt6 = -2E-2;

flt7 = 6.626e-24;

flt8 = .7; // invalid
flt9 = 7.; // invalid
flt10 = 3.e+20; // invalid

flt11 = 224_617.445_991_228;

ftl12 = inf;
ftl13 = +inf;
ftl14 = -inf;

flt15 = nan;
flt16 = +nan;
flt17 = -nan;
```

## Booleans
- libconfigfile does not support booleans. If `0` and `1` is good enough for C programmers, it's good enough for us.

## Arrays
- Arrays are square brackets with values inside. Elements are separated by commas.
- Arrays can contain values of the same data types allowed in key-value pairs. Values of different types may be mixed. Nested arrays are allowed.
- Arrays can span multiple lines. A trailing comma after the last value of the array is permitted. Whitespace between array values and commas is ignored.
```
integers = [ 1, 2, 3 ];
colors = [ "red", "yellow", "green" ];

nested_array_of_ints = [ [ 1, 2 ], [ 3, 4, 5 ] ];
nested_mixed_array = [ [ 1, 2 ], [ "a", "b", "c" ] ];

numbers = [ 0.1, 0.2, 0.5, 1, 2, 5, "one", "two", "five" ];

integers2 = [
    2, 3, 3
]

integers3 = [
    1,
    2,
    3,
]
```

## Maps
- Maps are collections of key-value pairs, enclosed by curly brackets.
- Anything between the curly brackets is considered to be within the scope of the map.
- Maps may be nested.
- Keys may not share names with other keys in the same direct scope.
- Empty maps are allowed and simply have no key-value pairs within them.
- The ordering of objects within a map is not preserved (for performance reasons).
- The top-level map, also called the root map, starts at the beginning of the document and ends at end-of-file, encompassing everything else in its scope. Unlike other maps, it is nameless.
```
map = {};

map_1 = {
    key1 = "some string";
    key2 = 123;
};

map_2 = {
    key1 = "another string";
    key2 = 456;
};

parent = {
    key = 1;
    child = {
        key = 2;
    };
};

map_3 = {
    key = "value";
};

map_3 = { // invalid
    key = "value";
};
```

## Directives
- Directives are defined by an `@` character followed by the name of the directive, and optionally (depending on the specific directive), arguments.
- Directives may only be found in the root map.
- The entire directive must appear on one line. A directive and its arguments must not be followed by any other text on the same line.
- If arguments are present, at least one whitespace characters is required between the end of the directive name and the start of the arguments. All other whitespace between the directive name and the arguments is ignored.
- Arguments are formatted and delimited according to the rules of the specific directive.
```
@directive "argument1"
@ directive "argument1"

@directive
"argument1" // invalid

key = "value"; @directive "argument1" // invalid

@directive"argument1" // invalid
```

### Version Directive
- This directive ensures that the parser and the config file are using the same version of the syntax specification.
- This directive takes one required argument: a release version number or git version corresponding to the syntax specification being used, enclosed by double quotes.
- If the parser detects a syntax specification version that it does not know how to parse, an error will be thrown.
- There should only be one version directive per file. If there happens to be multiple (ex. when using the include directive), the values should be identical.
- At the top of this document can be found the syntax specification version that it is describing.
- To obtain the current git version of libconfigfile, run the following command from within the git repository.
    - `printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"`
```
// release version
@version "1.0.0"

// git version
@version "r82.1fd9244"
```

### Include Directive
- This directive directs the parser to inline the contents of another file at the location of the directive.
- This directive takes one required argument: the path of the file to be inlined, enclosed by double quotes.
- The file path string supports the same escape characters as regular key-value strings; adjacent strings will not be concatenated.
- Any included files must be syntactically valid by themselves; i.e. they must be parsable without any additional surrounding context.
- Currently the parser does not check for recursive includes. You have been warned! If the parser segfaults due to a stack overflow, you have only yourself to blame!
```
// main.conf
key1 = "value";
@include "another_file.conf";
key2 = "value";

// another_file.conf
map = {
    key1 = "value";
    key2 = "value";
};

// result
key1 = "value"
map = {
    key1 = "value";
    key2 = "value";
};
key2 = "value"
```

## Filename Extension
- Config files require no particular filename extention, but if one is desired, `.conf` is recommended.

## Thanks
- The libconfigfile syntax was inspired by the following.
    - TOML: https://github.com/toml-lang/toml
    - libconfig: https://github.com/hyperrealm/libconfig
