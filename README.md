# Format Buffer

Format Buffer is a very lightweight header only library to write a format string to a buffer. It only supports the most basic format specifiers and floating point formats are omitted on purpose with the exception of hexadecimal binary scientific notation to keep things simple. 

## Usage

A format specifier follows this prototype: '%specifier'

| Specifier | Description                                              |
| --------- | -------------------------------------------------------- |
| d         | Signed Decimal Integer                                   |
| u         | Unsigned Decimal Integer                                 |
| x         | Integer in Hexadecimal Format                            |
| b         | Integer in Binary Format                                 |
| c         | Single Character                                         |
| s         | String of Characters (null-terminated)                   |
| a         | Floating Point in Hexadecimal Binary Scientific Notation |
| %         | The '%' Character                                        |

## Examples

```
// prints "2136"
FormatBuffer<1024> buf{"%d", 2136};
std::cout << buf.c_str() << std::endl;
```

```
// prints "0x1.91eb86p+1"
char buf[1024];
FormatBuffer::print(buf, "%a", 3.14f);
std::cout << buf.c_str() << std::endl;
```
