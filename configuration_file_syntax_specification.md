# Configuration File Syntax Specification

Version: 0

## Contents
- [Spec](#spec)
- [Comments](#comments)
- [Key-Value Pairs](#key-value-pairs)
- [Strings](#strings)
- [Integers](#integers)
- [Floats](#floats)
- [Booleans](#booleans)
- [Arrays](#arrays)
- [Sections](#sections)
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
```
key1 = "value";
key2 = ; // invalid
key3 = "value" // missing semicolon
= "value"; // invalid

key_4 = "value";
1234 = "value";

name = "Jasper";
name = "Young"; // invalid

(section1)
{
    name = "jasper1378"; // valid
}
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

float_8 = .7;
float_9 = 7.;
float_10 = 3.e+20;

flt11 = 224_617.445_991_228;

sf1 = inf;
sf2 = +inf;
sf3 = -inf;

sf4 = nan;
sf5 = +nan;
sf6 = -nan;
```

## Booleans
- libconfigfile does not support booleans. If `0` and `1` is good enough for C programmers, it's good enough for us.

## Arrays
- Arrays are square brackets with values inside. Elements are separated by
  commas.
- Arrays can contain values of the same data types allowed in key-value pairs. Values of different types may be mixed. Nested arrays are allowed.
- Arrays can span multiple lines. A trailing comma after the last value of the array is permitted. Whitespace between array values and commas is ignored.
```
integers = [ 1, 2, 3 ];
colors = [ "red", "yellow", "green" ];

nested_array_of_ints = [ [ 1, 2], [ 3, 4, 5 ] ];
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

## Sections
- Sections are collections of key-value pairs. They are defined by a name enclosed by round brackets, followed by the associated key-value pairs and/or nested sections enclosed by curly brackets.
- Anything between the curly brackets is considered to be within the scope of the section.
- Sections may be nested.
- Keys/sections may not share names with other keys/sections in the same direct scope.
- Naming rules for sections are the same as for keys.
- Section names must appear completely on one line.
- Whitespace between the opening/closing round bracket and the section name is allowed and will be ignored.
- Empty sections are allowed and simply have no key-value pairs and/or sections within them.
- Empty section names are not permitted.
- The ordering of objects within a section is not preserved (for performance reasons).
- The top-level section, also called the root section, starts at the beginning of the document and ends at end-of-file, encompassing everything else in its scope. Unlike other sections, it is nameless.
```
(section)
{
}

(section_1)
{
    key1 = "some string";
    key2 = 123;
}

(section_2) {
    key1 = "another string";
    key2 = 456;
}

(parent)
{
    key = 1;
    (child)
    {
        key = 2;
    }
}

(section_3)
{
    key = "value";
}

(section_3) // invalid
{
    key = "value";
}
```

## Directives
- Directives are defined by an `@` character followed by the name of the directive, and optionally (depending on the specific directive), arguments.
- The entire directive must appear on one line. A directive and its arguments must be the only text present on its line.
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
- This directive takes one required argument: a release version number or git version corresponding to the syntax specification being used.
- If the parser detects a syntax specification version that it does not know how to parse, an error will be thrown.
- There should only be one version directive per file. If there happens to be multiple (ex. when using the include directive), the values should be identical.
- At the top of this document can be found the syntax specification version that it is describing.
- To obtain the current git version of libconfigfile, run the following command from within the git repository.
    - `printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"`
```
// release version
@version 1.0.0

// git version
@version r82.1fd9244
```

### Include Directive
- This directive directs the parser to inline the contents of another file at the location of the directive.
- This directive takes one required argument: the path of the file to be inlined, enclosed by double quotes.
- The file path string supports the same escape characters as regular key-value strings; adjacent strings will not be concatenated.
- Currently the parser does not check for recursive includeds. You have been warned! If the parser segfaults due to a stack overflow, you only have yourself to blame!
```
// main.conf
key1 = "value";
@include "another_file.conf";
key2 = "value";

// another_file.conf
(section)
{
    key1 = "value";
    key2 = "value";
}

// result
key1 = "value"
(section)
{
    key1 = "value";
    key2 = "value";
}
key2 = "value"
```

## Filename Extension
- Config files require no particular filename extention, but if one is desired, `.conf` is recommended.

## Thanks
- The libconfigfile syntax was inspired by the following.
    - TOML: https://github.com/toml-lang/toml
    - libconfig: https://github.com/hyperrealm/libconfig
