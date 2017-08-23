INI Parser
==========

## Usage

```c
FILE *f = fopen("config.ini", "r");
if(!f) {
  /* error */
}

int type;
do {
  char key[128], value[128];
  type = parse_ini_file(f, &key[0], sizeof(key), &value[0], sizeof(value));
  if(type == INI_SECTION) {
    /* just received a section such as '[key]' or '[key "value"]' */
  } else if(type == INI_VALUE) {
    /* just received a key=value pair */
  }
} while(type);

fclose(f);
```

## Tests

This parser is covered by a comprehensive set of unit tests.

Run `make check` to compile and run the test suite.

## Installation

Just include `ini_parser.c` and `ini_parser.h` in your project. The only
dependency is the standard C library.

## License

Copyright (c) 2015 Harry Jeffery

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
