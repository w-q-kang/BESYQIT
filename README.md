# BESYQIT

## Introduction
BESYQIT is an esoteric/weird programming language. It is of course not intended for mainstream programming. Trying to solve a program using it should rather be viewed as a kind of brain-teaser. BESYQIT isn't as hard as BF or even Malbolge, so it should in principle be possible to solve your problem. However, the solution will often not be as straightforward or obvious as with a mainstream language. In a certain sense BESYQIT is the successor of the language SOPL that I already published some time ago.

To get acquainted with the programming style needed for BESYQIT programs you should look at some sample programs - see remarks below - and read the documentation (in PDF format).

The repository contains an interpreter written in C++ as well as sample programs.

## Motivation

After implementing the language SOPL and trying it out for some time it became clear to me that the language wasn’t close enough to natural languages yet: Modifiers are not part of the word they are affixed to but remain somewhat separate since they are connected by dashes or tilde.

I furthermore have to admit that English is not a beautiful language (neither in writing nor in pronunciation) so in order to create a beautiful language I had to abandon it and implement a totally different set of verbs/references/modifiers. Instead of trying to use a beautiful language that already exists I decided to invent a new language (conlang) which I call ‘Juubes’. The programming language that was derived from it is called ‘Besyqit’. Though vocabulary and grammar are different from SOPL, the basic capabilities are quite similar. 
For the purpose of programming the language need not be complete, however, to better motivate the words chosen I supply an introduction to Juubes (see Juubes – Introduction.pdf) and give a basic vocabulary file (juubes-fiiar.csv).

As a result it should be possible to read a program aloud in that language. Note that using a natural language may introduce ambiguities since the same word may be used with different meanings in Juubes. However, Besyqit itself is freed from these ambiguities by defining exact meanings to the words used in a specific context.

As with probably all programming languages the grammar and word usage is a little bit different from the ‘natural’ language itself. 

Though the original motivation for inventing Juubes was just to supply words for the programming language Besyqit, it should also be thought of as a spoken language. Since in the standard variety of Besyqit error messages are given in Juubes, you might want to acquire a basic knowledge of Juubes (or look up the translations in the source code itself). Alternatively you can use the switch -prat when calling the BESYQIT interpreter to get English error messages (and switch off the compliance check for user defined paragraph names etc.)

To avoid confusion: please note that Besyqit uses base30 for numbers (base30.30 cf. Juubes introduction) and outputs all numbers written out (in Juubes).

## Build
You should compile the source code with C++17 or later. To avoid crashes for programs making heavy use of recursion increase heap and stack by setting the appropriate linker options. I use the following settings:
-Wl,--stack,700000000
-Wl,--heap,500000000
Note that I still prefer to include debug code and suggest you do the same when compiling the code. 
### Windows
You should have GCC (including the compiler g++) installed first. Then run the build script:

```batch
make.bat
```

Alternatively, create directories `obj`, `obj/src`, `obj/Debug`, `obj/Debug/src`, `bin` and `bin/Debug`, then execute the following commands

```batch
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c main.cpp -o obj\Debug\main.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Basics.cpp -o obj\Debug\src\Basics.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Compliance.cpp -o obj\Debug\src\Compliance.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Datetime.cpp -o obj\Debug\src\Datetime.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\globals.cpp -o obj\Debug\src\globals.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Interpreter.cpp -o obj\Debug\src\Interpreter.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Item.cpp -o obj\Debug\src\Item.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Juubes.cpp -o obj\Debug\src\Juubes.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Lexer.cpp -o obj\Debug\src\Lexer.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Log.cpp -o obj\Debug\src\Log.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Opcode.cpp -o obj\Debug\src\Opcode.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Paragraph.cpp -o obj\Debug\src\Paragraph.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Parser.cpp -o obj\Debug\src\Parser.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Preprocess.cpp -o obj\Debug\src\Preprocess.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Rowitem.cpp -o obj\Debug\src\Rowitem.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Second.cpp -o obj\Debug\src\Second.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Sentence.cpp -o obj\Debug\src\Sentence.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Third.cpp -o obj\Debug\src\Third.o
g++.exe  -o bin\Debug\BESYQIT.exe obj\Debug\main.o obj\Debug\src\Basics.o obj\Debug\src\Compliance.o obj\Debug\src\Datetime.o obj\Debug\src\globals.o obj\Debug\src\Interpreter.o obj\Debug\src\Item.o obj\Debug\src\Juubes.o obj\Debug\src\Lexer.o obj\Debug\src\Log.o obj\Debug\src\Opcode.o obj\Debug\src\Paragraph.o obj\Debug\src\Parser.o obj\Debug\src\Preprocess.o obj\Debug\src\Rowitem.o obj\Debug\src\Second.o obj\Debug\src\Sentence.o obj\Debug\src\Third.o  -Wl,--stack,700000000 -Wl,--heap,500000000  
```

After successfully compiling you should be all set. Try executing a sample program like so:

```batch
bin/Debug/BESYQIT.exe -x -prat samples/hello_world.bsqt
```

### macOS/Linux
This requires `clang++` to be installed. Run the build script:

```shell
./make.sh
```

Alternatively, create directories `obj`, `obj/src`, `obj/Debug`, `obj/Debug/src`, `bin` and `bin/Debug`, then execute the following commands

```shell
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c main.cpp -o obj/Debug/main.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Basics.cpp -o obj/Debug/src/Basics.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Compliance.cpp -o obj/Debug/src/Compliance.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Datetime.cpp -o obj/Debug/src/Datetime.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/globals.cpp -o obj/Debug/src/globals.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Interpreter.cpp -o obj/Debug/src/Interpreter.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Item.cpp -o obj/Debug/src/Item.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Juubes.cpp -o obj/Debug/src/Juubes.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Lexer.cpp -o obj/Debug/src/Lexer.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Log.cpp -o obj/Debug/src/Log.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Opcode.cpp -o obj/Debug/src/Opcode.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Paragraph.cpp -o obj/Debug/src/Paragraph.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Parser.cpp -o obj/Debug/src/Parser.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Preprocess.cpp -o obj/Debug/src/Preprocess.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Rowitem.cpp -o obj/Debug/src/Rowitem.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Second.cpp -o obj/Debug/src/Second.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Sentence.cpp -o obj/Debug/src/Sentence.o
clang++ -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src/Third.cpp -o obj/Debug/src/Third.o
clang++  -o bin/Debug/besyqit obj/Debug/main.o obj/Debug/src/Basics.o obj/Debug/src/Compliance.o obj/Debug/src/Datetime.o obj/Debug/src/globals.o obj/Debug/src/Interpreter.o obj/Debug/src/Item.o obj/Debug/src/Juubes.o obj/Debug/src/Lexer.o obj/Debug/src/Log.o obj/Debug/src/Opcode.o obj/Debug/src/Paragraph.o obj/Debug/src/Parser.o obj/Debug/src/Preprocess.o obj/Debug/src/Rowitem.o obj/Debug/src/Second.o obj/Debug/src/Sentence.o obj/Debug/src/Third.o  -Wl,-stack_size -Wl,0x30000000 -Wl,-stack_size -Wl,0x20000000  
```

After successfully compiling you should be all set. Try executing a sample program like so:

```shell
bin/Debug/besyqit -x -prat samples/hello_world.bsqt
```

## Structure
The basic structure of a BESYQIT program is as follows:
- It starts with a header, basically consisting of a paragraph call and ending with an empty line.
- Then a series of paragraphs follows, each paragraph is introduced with its name followed by a colon on a separate line and a subsequent series of sentences. Paragraphs are terminated by an empty line.
- Sentences are series of words where the last word is immediately followed by a dot. The last word of a sentence is considered a verb and equivalent to a function name while the preceding words serve as parameters.
- Essentially each sentence is regarded as an expression and has the value of the evaluated expression.
- You may reference values of preceding sentences (inside the same paragraph) by special reference words like 'yii', 'ya' etc.

For a thorough description of the language please see the PDF documentation in the docs sub-directory. Since - in contrast to the predecessor SOPL - BESYQIT doesn't use English words any more you might also want to get acquainted with the conlang 'Juubes' that BESYQIT words/commands are based on, see the introduction to Juubes in PDF format.

## Design
The only design principle I am trying to follow is KISS: Keep it simple and stupid. Sure the code isn't always as stupid as it could be but I keep trying...

## Samples
The 'samples' directory lists the solutions to a few 'standard' tasks. Might be extended in the future... 

There is also a 'features' directory which contains small programs that mostly do not have a meaning in themselves but simply demonstrate the features of the language. You should make yourself familiar with the examples listed there and try to understand the results by running them to get a better understanding of the language.

## Issues
If you find that something is wrong or should be changed for whatever reason please submit an issue.
