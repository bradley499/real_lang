# Real language
The Real language is a programming language designed for portability, primarily for embedded devices. Featuring a limited codebase for logical operations.

The Real language program is designed for quick relation, logical operations, to support mathematical and computation processes.

Everything is **Real**, meaning that the only data type to store any values in is a floating-point/real number.

## Getting started
You can build the packager of the Real language in by making the program in the `real_lang/packager` directory. The program can be built by using the standard 'GNU make' program, and can be simply done by using the command `make`.

## How to use
### Packager
Once you have compiled the packager, it will be compiled and outputted into the relative `build/` directory, navigate to it; now you can use it in the default convention of running any binary program. Now you can use the packager to package the Real language into a portable interpretive redistributable package. It is suggested that you try `./packager --help` to see a list of commands.

Usage: `[options] {-i input_path} ... {-o output_path}`
| Argument | Meaning |
|-|-|
| -i, <nobr>--input</nobr> &lt;file&gt; | Expects the Real language input file path to be subsequently appended after this argument |
| -o, <nobr>--output</nobr> &lt;file&gt; | Expects the file path of the package output file to be subsequently appended after this statment |
| <nobr>--no-color</nobr>, <nobr>--no-colour</nobr> | Removes all colour from any outputs |
| <nobr>--raw</nobr> | Outputs the raw packaged program to the console |
| -v, <nobr>--verbose</nobr> | Gives a verbose output of what is currently occuring |
| <nobr>--version</nobr> | Prints out the version number, release number, and compilation time |

<br>

###### Last updated: 06/11/2020 - Bradley Marshall